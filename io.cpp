#include "ARequestHandler.hpp"
#include "FdManager.hpp"
#include "includes.hpp"

// TODO: small functions: clear_client()

// 1: ok
// 0: connection closed by the client
// -1: read() error
int recv_from_client(int socket, FdManager &table)
{
    char buffer[BUFFER_SIZE];
    Client &client = *table[socket].client;
    int max_read;
    int recvd_bytes;

    max_read = BUFFER_SIZE - client.received_data.size();
    if (max_read <= 0)
        return (1);
    recvd_bytes = read(socket, buffer, max_read);
    if (recvd_bytes == -1)
    {
        perror("read");
        return (-1);
    }
    if (recvd_bytes == 0) // connection closed by the client
    {
        // clear Client
        close(socket);
        // Obs: request handler must clear clients ressources
        if (client.state == handling_response)
            client.ongoing_response->abort();
        delete &client;
        table.remove_fd(socket);

        // debug // TODO: include client's address in the Client struct
        std::cout << "Connection at socket " << socket << " was closed by the client " << std::endl;

        return (0);
    }

    // debug
    std::cout << "Received " << recvd_bytes << " bytes from client at socket " << socket << std::endl;
    client.received_data.append(buffer, recvd_bytes);
    return (1);
}

// Request handler's state should be "s_sending_file"
int read_from_file(int fd_file, FdManager &table)
{
    char buffer[BUFFER_SIZE];
    int max_read;
    int read_bytes;
    Client &client = *table[fd_file].client;

    max_read = BUFFER_SIZE - client.unsent_data.size();
    if (max_read <= 0)
        return (1);
    read_bytes = read(fd_file, buffer, max_read);
    if (read_bytes == -1)
    {
        perror("read");
        return (-1);
    }
    // EOF
    if (read_bytes == 0)
    {
        table[fd_file].is_EOF = true;
        return (0);
    }
    client.unsent_data.append(buffer, read_bytes);
    table.set_pollout(client.socket);

    // debug
    std::cout << read_bytes << " bytes were read from fd " << fd_file << " destinated to client at socket "
              << client.socket << std::endl;

    return (1);
}

// -1: write() error
// 0: no data do send
// 1: ok
int send_to_client(int socket, FdManager &table)
{
    Client &client = *table[socket].client;
    int bytes_sent;

    if (client.unsent_data.empty()) // POLLOUT should not be set in the first place...
        return (0);
    bytes_sent = write(socket, client.unsent_data.data(), client.unsent_data.size());
    if (bytes_sent == -1)
    {
        perror("write"); // which is not allowed by the subject!
        return (-1);
    }
    // debug
    std::cout << bytes_sent << " bytes were sent to client at socket " << socket << std::endl;
    client.unsent_data.erase(0, bytes_sent);
    if (client.unsent_data.empty())
        table.unset_pollout(client.socket);
    return (1);
}

// transfer data from client's request body to cgi input
int write_to_cgi(int fd_cgi_input, FdManager &table)
{
    Client &client = *table[fd_cgi_input].client;
    int bytes_written;

    if (client.req_body_data.empty())
        return (0);
    bytes_written = write(fd_cgi_input, client.req_body_data.data(),
        client.req_body_data.size());
    if (bytes_written == - 1)
    {
        perror("write");
        return (-1);
    }
    client.received_data.erase(0, bytes_written);
    if (client.req_body_data.empty())
        table.unset_pollout(fd_cgi_input);
    
    // debug
    std::cout << bytes_written << " bytes were sent to CGI's input at fd "
        << fd_cgi_input << std::endl;
    
    return (1);
}
