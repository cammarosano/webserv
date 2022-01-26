#ifndef INCLUDES_HPP
# define INCLUDES_HPP


// C++ stuff
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

// C stuff
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // struct sockadd_in
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>

// states
enum e_states
{
	get_header
};

// structs
struct Client
{
	std::string unprocessed_data;
	e_states recv_state;
};

struct HttpRequest
{
	std::string request_line;
	std::map<std::string, std::string> header_fields;
	std::string body;
};

// function prototypes
int get_listening_socket(std::string host_IP, unsigned short port);
int accept_connection(int listen_socket, std::map<int,Client> &m);
int handle_incoming_data(int socket, std::map<int, Client> &m);
int handle_outbound_data(int socket);

#endif
