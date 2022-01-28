#ifndef INCLUDES_HPP
# define INCLUDES_HPP


// C++ stuff
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <queue>
#include <algorithm>

// C stuff
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/stat.h>

// my classes
#include "Poll_array.hpp"

// states
enum e_recv_state
{
	get_header
};

enum e_send_state
{
	start, load_ressource, data_ready
};

// structs
struct HttpResponse
{
	// status-line
	std::string http_version, status_code_phrase;

	// header-fields
	std::map<std::string, std::string> header_fields;

	int fd_ressource;
	size_t bytes_left;

	e_send_state send_state;
	std::string unsent_data;
};

# define BUFFER_SIZE 1024
struct Client
{
	e_recv_state recv_state;
	std::string received_data;

	e_send_state send_state;
	std::string unsent_data;

	std::queue<HttpResponse> response_queue;


};

struct HttpRequest
{
	std::string method, request_target, http_version;
	std::map<std::string, std::string> header_fields;
	std::string body;
};

// function prototypes
int get_listening_socket(std::string host_IP, unsigned short port);
int accept_connection(int listen_socket, std::map<int, Client> &m);
int handle_incoming_data(int socket, Client &client);
int handle_outbound_data(int socket, Client &client);
int handle_get_request(HttpRequest &request, Client &client);


// IO
int recv_from_client(int socket, std::map<int, Client> &clients_map);
int send_to_client(int socket, std::map<int, Client> &clients_map);

// utils
std::string long_to_str(long nb);

#endif
