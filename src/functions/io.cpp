#include "includes.hpp"

// read() from client's socket
// data is stored in Client::received_data
// if client was in "idle" state, state is updated to "incoming_request"
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
		if (DEBUG)
			perror("read"); // REMOVE THIS BEFORE PUSH
		remove_client(client, table, "webserv(read error)");
		return;
	}
	if (recvd_bytes == 0) // connection closed by the client
	{
		if (DEBUG)
			std::cout << "socket " << socket << ": ";
		remove_client(client, table, "peer");
		return;
	}
	client.received_data.append(buffer, recvd_bytes);
	if (client.is_idle())
		client.update_state();

	// debug
	if (DEBUG)
		std::cout << "Received " << recvd_bytes
				  << " bytes from client at socket " << socket << std::endl;
}

// write() to Client's socket.
// Client::unsent_data is the data source.
// Clients tagged with "disconnect_after_send" are removed if the unsent_data
// buffer is emptied
void send_to_client(int socket, FdManager &table)
{
	Client &client = *table[socket].client;
	int bytes_sent;

	if (!client.unsent_data.empty())
	{
		bytes_sent =
			write(socket, client.unsent_data.data(), client.unsent_data.size());
		if (bytes_sent == -1 ||
			bytes_sent == 0) // error (Obs: should we consider 0 an error too?)
		{
			if (DEBUG)
				perror("write"); // REMOVE BEFORE PUSH
			remove_client(client, table, "webserv (write error)");
			return;
		}
		client.unsent_data.erase(0, bytes_sent);
		// debug
		if (DEBUG)
			std::cout << bytes_sent << " bytes were sent to client at socket "
					  << socket << std::endl;
	}
	if (client.unsent_data.empty())
	{
		table.unset_pollout(client.socket);
		if (client.disconnect_after_send)
		{
			remove_client(client, table,
						  "webserv (disconnect after response sent)");
			return;
		}
	}
}

// reads data from a fd (file or pipe with CGI output)
// copies it into Client:unsent_data buffer
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
		if (DEBUG)
			perror("read"); // REMOVE BEFORE PUSH
		remove_client(client, table, "webserv (read error)");
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
		std::cout << read_bytes << " bytes were read from fd " << fd
				  << " destinated to client at socket " << client.socket
				  << std::endl;
}

// write data from Client's decoded_body to fd (file for uploads, or pipe to
// CGI input)
void write_to_fd(int fd, FdManager &table)
{
	Client &client = *table[fd].client;
	int bytes_written;

	if (client.decoded_body.empty())
	{
		table.unset_pollout(fd);
		return;
	}
	bytes_written =
		write(fd, client.decoded_body.data(), client.decoded_body.size());
	if (bytes_written == -1 ||
		bytes_written == 0) // should we consider write 0 error?
	{
		if (DEBUG)
			perror("write"); // REMOVE BEFORE PUSH
		remove_client(client, table, "webserv (write error)");
		return;
	}
	client.decoded_body.erase(0, bytes_written);
	if (client.decoded_body.empty())
		table.unset_pollout(fd);

	// debug
	if (DEBUG)
		std::cout << bytes_written << " bytes were sent to fd " << fd
				  << std::endl;
}
