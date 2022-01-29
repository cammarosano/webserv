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

// forward declaration
class Fd_table;

// states
enum e_recv_state
{
	get_header
};

enum e_response_state
{
	sending_header, start_send_file, sending_file, send_file_complete, done
};

enum e_body_source
{
	none, file
};

// structs
struct HttpResponse
{
	e_response_state state;

	// Http header
	// status-line
	std::string http_version, status_code_phrase;
	// header-fields
	std::map<std::string, std::string> header_fields;
	// complete header
	std::string header_str;

	// Ressources
	e_body_source source_type;	
	// file or cgi
	int fd_read;
	// size_t bytes_left;

};

# define BUFFER_SIZE 4096
struct Client
{
	int socket; // only needed for debugging pourposes?

	e_recv_state recv_state;
	std::string received_data;

	std::string unsent_data;

	std::queue<HttpResponse> response_q;

	Client(int socket): socket(socket), recv_state(get_header) {}

};

enum e_fd_type
{
	fd_listening_socket,
	fd_client_socket,
	fd_file,
	fd_cgi_output,
	fd_cgi_input
};

struct fd_info
{
	e_fd_type		type;
	Client 			*client;
	HttpResponse	*response;

	fd_info(): client(NULL), response(NULL) {}
};


struct HttpRequest
{
	Client &client;
	std::string method, request_target, http_version;
	std::map<std::string, std::string> header_fields;
	std::string body;

	HttpRequest(Client &client): client(client) {}
};

// function prototypes
int get_listening_socket(std::string host_IP, unsigned short port);
int accept_connection(int listen_socket, Fd_table &table);
int handle_incoming_data(int socket, Client &client);
int handle_outbound_data(int socket, Client &client);
int handle_get_request(HttpRequest &request, Client &client);


// IO
int recv_from_client(int socket, Fd_table &table);
int read_from_file(int fd_file, Fd_table &table);
int send_to_client(int socket, Fd_table &table);

// process
int process_incoming_data(Fd_table &table, std::queue<HttpRequest> &requests);
int handle_requests(std::queue<HttpRequest> &q);
int handle_responses(Fd_table &table);

// utils
std::string long_to_str(long nb);
std::string & str_tolower(std::string &s);
void print_request(HttpRequest &request);

#endif
