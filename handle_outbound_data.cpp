#include "includes.hpp"

int handle_outbound_data(int socket)
{
	std::string hello_msg = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 6\r\n\r\n"
		"Hello!";
	
	send(socket, hello_msg.c_str(), hello_msg.size(), 0);


	return (0);
}