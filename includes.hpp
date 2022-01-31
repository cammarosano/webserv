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

// states
enum e_recv_state
{
	get_header, get_body
};

enum e_response_state
{
	sending_header, start_send_file, sending_file, send_file_complete, done,
	sending_qstring // experimental!
};

enum e_body_source
{
	none, file, qstring
};

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

	HttpResponse(): state(sending_header) {}
};

struct Client
{
	int socket; // only needed for debugging pourposes?
	std::list<Vserver> &vservers;

	e_recv_state recv_state;
	std::string received_data;
	std::string unsent_data;

	// info needed for when receiving the body
	int body_bytes_left;
	std::string processed_data;

	std::queue<HttpResponse> response_q;

	Client(int socket, std::list<Vserver> &vservers):
	socket(socket), vservers(vservers), recv_state(get_header) {}

};

struct HttpRequest
{
	Client &client;
	std::string method, target, http_version;
	std::map<std::string, std::string> header_fields;
	std::string body;

	HttpRequest(Client &client): client(client) {}
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

// process
int process_incoming_data(FdManager &table, std::queue<HttpRequest> &requests);
int handle_requests(std::queue<HttpRequest> &q);
int handle_responses(FdManager &table);

// resolving
Vserver & resolve_vserver(HttpRequest &request);
Route * resolve_route(Vserver &vserver, std::string &request_target);

// responses
int issue_404_response(HttpRequest &request);
int issue_403_response(HttpRequest &request);
int issue_200_response(HttpRequest &request, int fd_file, struct stat &sb);

// utils
std::string long_to_str(long nb);
std::string & str_tolower(std::string &s);
void print_request(HttpRequest &request);
std::string & remove_trailing_spaces(std::string &s);

// weird stuff
int issue_200_post_resp(HttpRequest &request, int payload_size);

#endif
