#include "includes.hpp"
#include "Fd_table.hpp"

// 1: ok
// 0: connection closed by the client
// -1: read() error
int recv_from_client(int socket, Fd_table &table)
{
	char buffer[BUFFER_SIZE + 1];
	Client &client = *table[socket].client;

	int max_read = BUFFER_SIZE - client.received_data.size();
	if (max_read <= 0)
		return (1);
	int ret = read(socket, buffer, max_read);
	if (ret == -1)
	{
		perror("read");
		return (-1);
	}
	if (ret == 0) // connection closed by the client
	{
		table.remove_client(socket);
		// debug
		std::cout << "Connection at socket " << socket
				<< " was closed by the client " << std::endl;

		return (0);
	}

	// debug
	std::cout << "Received " << ret << " bytes from client at socket "
				<< socket << std::endl;

	buffer[ret] = '\0';
	client.received_data += buffer;
	return (1);
}

// response.state should be "sending_file"
int recv_from_file(int fd_file, Fd_table &table)
{
	char buffer[BUFFER_SIZE + 1];
	int max_read;
	Client &client = *table[fd_file].client;

	max_read = BUFFER_SIZE - client.unsent_data.size();
	if (max_read <= 0)
		return (1);
	int ret = read(fd_file, buffer, max_read);
	if (ret == -1)
	{
		perror("read");
		return (-1);
	}
	if (ret == 0) // EOF
	{
		HttpResponse &response = *table[fd_file].response;
		response.state = send_file_complete; // responde handler will close the fd
		return (0);
	}
	// debug
	std::cout << ret << " bytes were read from file at fd " << fd_file
				<< std::endl;
	buffer[ret] = '\0';
	client.unsent_data += buffer;
	return (1);
}

// -1: write() error
// 0: no data do send
// 1: ok
int send_to_client(int socket, Fd_table &table)
{
	Client &client = *table[socket].client;
	int bytes_sent;

	if (client.unsent_data.empty())
		return (0);
	bytes_sent = write(socket, client.unsent_data.data(),
							client.unsent_data.size());
	if (bytes_sent == -1)
	{
		perror("write"); // which is not allowed by the subject!
		return (-1);
	}
	// debug
	std::cout << bytes_sent << " bytes were sent to client at socket "
			<< socket << std::endl;
	client.unsent_data.erase(0, bytes_sent);
	return (1);
}
