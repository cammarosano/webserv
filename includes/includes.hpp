#ifndef INCLUDES_HPP
# define INCLUDES_HPP

// C++ stuff
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <queue>
#include <list>
#include <set>
#include <utility>

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
#include <stdlib.h> // atoi

# define BUFFER_SIZE 4096

// forward declaration
class FdManager;
class ARequestHandler;
struct HttpRequest;

// structs

struct Route
{
	std::string				prefix;	// location
	std::list<std::string>	accepted_methods;
	// todo: redirection
	std::string				root;
	bool					auto_index; // directory listing
	std::string				default_index;

	// cgi
	std::string				cgi_extension;
	std::string				cgi_interpreter; // program name/path

	// upload
	bool					upload_accepted;
	std::string				upload_dir;	// where to store

	Route(std::string prefix): prefix(prefix) {}
};

typedef	std::pair<std::string, unsigned short> ip_port;

struct Vserver
{
	ip_port					listen;
	std::list<std::string>	server_names;
	std::string				default_404;	// default error page
	std::string				default_403;	// default error page
	// etc...
	size_t					body_size_limit;
	std::list<Route>		routes;
};

struct HttpResponse
{
	// status-line
	std::string http_version, status_code_phrase;
	// header-fields
	std::map<std::string, std::string> header_fields;
};

enum e_client_state
{
	recv_header, handling_response
};

struct Client
{
	int socket; // only needed for debugging pourposes?
	std::list<Vserver> &vservers;

	e_client_state state;

	// buffers
	std::string received_data;
	std::string unsent_data;
	std::string processed_data;

	// ongoing response
	ARequestHandler *ongoing_response;

	// initialization
	Client(int socket, std::list<Vserver> &vservers):
	socket(socket), vservers(vservers), state(recv_header),
	ongoing_response(NULL) {}

};

// function prototypes

// setup
int setup(FdManager &table);
int get_listening_socket(std::string host_IP, unsigned short port);

// IO
int accept_connection(int listen_socket, FdManager &table);
int recv_from_client(int socket, FdManager &table);
int read_from_file(int fd_file, FdManager &table);
int send_to_client(int socket, FdManager &table);

// process requests
int check4new_requests(FdManager &table,
						std::list<ARequestHandler*> &req_handlers_lst);
int handle_requests(std::list<ARequestHandler*> &list);

// utils
std::string long_to_str(long nb);
std::string & str_tolower(std::string &s);
void print_request(HttpRequest &request);
std::string & remove_trailing_spaces(std::string &s);

#endif
