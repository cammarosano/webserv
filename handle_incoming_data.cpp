#include "includes.hpp"

#define BUFFER_SIZE 1024

int parse_header(std::string &header_str)
{
	// TODO
	(void)header_str;
	return (0);

}

int process_data(Client &client)
{
	if (client.recv_state == get_header)
	{
		// look for end-of-header delimiter: CRLF CRLF
		size_t pos = client.unprocessed_data.find("\r\n\r\n");
		if (pos == std::string::npos) // not found: header is incomplete
			return (2);
		
		std::string header_str = client.unprocessed_data.substr(0, pos);
		client.unprocessed_data.erase(0, pos + 4);

		std::cout << "header string: " << header_str << std::endl;
		parse_header(header_str);
		return (1);

	}

		return (2);


}

// returns -1 if error
// 0 if connection was closed by the client
// 1 if a request was received
// 2 if request is incomplete
int handle_incoming_data(int socket, std::map<int, Client> &m)
{
	Client &	client = m[socket];
	char		buffer[BUFFER_SIZE + 1];
	ssize_t		bytes_recvd;

	bytes_recvd = recv(socket, buffer, BUFFER_SIZE, 0);
	if (bytes_recvd == -1)
	{
		perror("recv");
		return (-1);
	}
	if (bytes_recvd == 0) // connection has been closed by the client
	{
		close(socket);
		std::cout << "Connection was closed by the client" << std::endl;
		return (0);
	}

	client.unprocessed_data += buffer;
	return (process_data(client));

}