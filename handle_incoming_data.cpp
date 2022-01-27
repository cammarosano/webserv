#include "includes.hpp"

#define BUFFER_SIZE 1024

// for debugging
void print_request(HttpRequest &request)
{
	std::cout << "method: " << request.method << '\n'
			<< "target: " << request.request_target << '\n'
			<< "http-version: " << request.http_version << '\n';

	for (std::map<std::string, std::string>::iterator it = request.header_fields.begin();
			it != request.header_fields.end(); it++)
		std::cout << it->first << ":" << it->second << '\n';
		
}

std::string & str_tolower(std::string &s)
{
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
		*it = tolower(*it);
	return (s);
}

int parse_header(std::string &header_str, HttpRequest &request)
{
	std::istringstream	stream(header_str);
	stream >> request.method;
	stream >> request.request_target;
	stream >> request.http_version;
	
	while (!stream.eof())
	{
		std::string line;
		getline(stream, line);
		size_t pos = line.find(':');
		if (pos == std::string::npos) 
			continue;
		std::string field_name = line.substr(0, pos);
		str_tolower(field_name);
		request.header_fields[field_name] = line.substr(pos + 1);
	}
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
		
		// header is complete
		std::string header_str = client.unprocessed_data.substr(0, pos);
		client.unprocessed_data.erase(0, pos + 4);

		// parse header and call method_handler
		HttpRequest request;
		parse_header(header_str, request);
		if (request.method == "GET")
		{
			handle_get_request(request, client);
		}

		return (1);

	}

	return (2);


}

// returns -1 if error
// 0 if connection was closed by the client
// 1 if a request was received
// 2 if request is incomplete
int handle_incoming_data(int socket, Client &client)
{
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