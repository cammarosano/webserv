#include "includes.hpp"

// close socket, delete Client object and remove from table
// (if ongoing response) send abort signal to Request Handler
void disconnect_client(Client &client, FdManager &table,
                        const char *who)
{
    // log
    std::cout << "Connection closed by " << who << ": " << client.ipv4_addr
            << " (" << client.host_name << ")" << std::endl;

    close(client.socket);
    if (client.ongoing_response)
    {
        client.ongoing_response->disconnect_client();
        client.ongoing_response->abort();
    }
    table.remove_fd(client.socket);
    delete &client;
}

void update_time_last_activ(Client &client)
{
    if (client.ongoing_response)
        client.ongoing_response->update_last_io_activ();
}

void recv_from_client(int socket, FdManager &table)
{
    char buffer[BUFFER_SIZE];
    Client &client = *table[socket].client;
    int max_read;
    int recvd_bytes;

    max_read = BUFFER_SIZE - client.received_data.size();
    if (max_read <= 0)
        return; // consider unsetting POLLIN for preventing overcharging server
    recvd_bytes = read(socket, buffer, max_read);
    if (recvd_bytes == -1) // error
    {
        if (DEBUG) perror("read"); // REMOVE THIS BEFORE PUSH
        disconnect_client(client, table, "webserv(read error)");
        return;
    }
    if (recvd_bytes == 0) // connection closed by the client
    {
        if (DEBUG) std::cout << "socket " << socket << ": ";
        disconnect_client(client, table, "peer");
        return;
    }
    client.received_data.append(buffer, recvd_bytes);

    // debug
    if (DEBUG)
        std::cout << "Received " << recvd_bytes <<
            " bytes from client at socket " << socket << std::endl;

}

// this is hacky
void log_cgi_output(char *buffer, int n_bytes)
{
    std::string str(buffer, n_bytes);
    size_t beg, end;
    beg = str.find("HTTP/1.1 ");
    if (beg != std::string::npos)
    {
        end = str.find('\n', beg);
        std::cout << "Response (CGI-generated): " << str.substr(beg, end)
            << std::endl;
    }
}

// reads data from a fd (file or pipe)
// copies it into Client's unsent data buffer
// if error, response is terminated and client disconnected
void read_from_fd(int fd, FdManager &table)
{
    char buffer[BUFFER_SIZE];
    int max_read;
    int read_bytes;
    Client &client = *table[fd].client;

    max_read = BUFFER_SIZE - client.unsent_data.size();
    if (max_read <= 0)
        return;
    read_bytes = read(fd, buffer, max_read);
    if (read_bytes == -1)
    {
        if (DEBUG) perror("read"); // REMOVE BEFORE PUSH
        disconnect_client(client, table, "webserv (read error)");
        return;
    }
    if (read_bytes == 0) // EOF
    {
        table[fd].is_EOF = true;
        return;
    }
    client.unsent_data.append(buffer, read_bytes);
    table.set_pollout(client.socket);

    // debug
    if (DEBUG)
        std::cout << read_bytes << " bytes were read from fd " << fd <<
            " destinated to client at socket " << client.socket << std::endl;

	// log CGI output
    if (table[fd].type == fd_cgi_output)
        log_cgi_output(buffer, read_bytes);
}

void send_to_client(int socket, FdManager &table)
{
    Client &client = *table[socket].client;
    int bytes_sent;

    if (client.unsent_data.empty()) // POLLOUT should not be set in the first place...
    {
        table.unset_pollout(client.socket);
        return;
    }
    bytes_sent = write(socket, client.unsent_data.data(),
                        client.unsent_data.size());
    if (bytes_sent == -1 || bytes_sent == 0) // error (Obs: should we consider 0 an error too?)
    {
        if (DEBUG) perror("write"); // REMOVE BEFORE PUSH
        disconnect_client(client, table, "webserv (write error)");
        return;
    }
    client.unsent_data.erase(0, bytes_sent);
    if (client.unsent_data.empty())
    {
        if (client.disconnect_after_send)
        {
            disconnect_client(client, table, "webserv(time-out)");
            return;
        }
        table.unset_pollout(client.socket);
    }
    update_time_last_activ(client);
    if (client.ongoing_response)
        client.ongoing_response->add_to_bytes_sent(bytes_sent);

    // debug
    if (DEBUG)
        std::cout << bytes_sent << " bytes were sent to client at socket "
                    << socket << std::endl;
}

// write data from Client's decoded_body to fd (file or pipe)
// if error, response is terminated and client disconnected
void write_to_fd(int fd, FdManager &table)
{
    Client &client = *table[fd].client;
    int bytes_written;

    if (client.decoded_body.empty())
    {
        table.unset_pollout(fd);
        return;
    }
    bytes_written = write(fd, client.decoded_body.data(),
        client.decoded_body.size());
    if (bytes_written == -1 || bytes_written == 0) // should we consider write 0 error?
    {
        if (DEBUG) perror("write"); // REMOVE BEFORE PUSH 
        disconnect_client(client, table, "webserv (write error)");
        return;
    }
    client.decoded_body.erase(0, bytes_written);
    if (client.decoded_body.empty())
        table.unset_pollout(fd);
    
    // debug
    if (DEBUG)
        std::cout << bytes_written << " bytes were sent to fd "
        << fd << std::endl;
}
