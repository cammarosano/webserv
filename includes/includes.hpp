#ifndef INCLUDES_HPP
#define INCLUDES_HPP

// debug stuff
#define RED "\033[0;31m"
#define RESET "\033[0m"
#define GREEN "\033[0;32m"
#define COUTDEBUG(x, c) std::cout << c << x << RESET << std::endl

// C++ stuff
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>

// C stuff
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>  // atoi
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>

#define BUFFER_SIZE 4096

// forward declarations
class FdManager;
class ARequestHandler;
struct HttpRequest;

// structs

// equivalent to nginx's "location"
struct Route {
    std::string prefix;  // location
    std::list<std::string> accepted_methods;
    // todo: redirection
    std::string root;
    bool auto_index;  // directory listing
    std::string default_index;

    // cgi
    std::string cgi_extension;
    std::string cgi_interpreter;  // program name/path

    // upload
    bool upload_accepted;
    std::string upload_dir;  // where to store

    // route error pages
    std::map<int, std::string> error_pages;

    Route(std::string prefix) : prefix(prefix) {}
};

typedef std::pair<std::string, unsigned short> ip_port;

struct Redirection {
    std::string location;
    int status_code;
};

// equivalent to nginx's "server"
struct Vserver {
    ip_port listen;
    bool redirected;
    std::list<std::string> server_names;
    std::string default_404;  // default error page
    std::string default_403;  // default error page
    Redirection redirect;

    // key is the code value is the page path ex 404 => 404.html
    std::map<int, std::string> err_pages;
    // etc...
    size_t body_size_limit;
    std::list<Route> routes;
};

struct HttpResponse {
    // status-line
    std::string http_version, status_code_phrase;
    // header-fields
    std::map<std::string, std::string> header_fields;
};

enum e_client_state { recv_header, handling_response };

struct Client {
    int socket;
    std::list<Vserver> &vservers;

    e_client_state state;

    // buffers
    std::string received_data;
    std::string unsent_data;
    std::string processed_data;

    // ongoing response
    ARequestHandler *ongoing_response;

    // initialization
    Client(int socket, std::list<Vserver> &vservers)
        : socket(socket),
          vservers(vservers),
          state(recv_header),
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
                       std::list<ARequestHandler *> &req_handlers_lst);
int handle_requests(std::list<ARequestHandler *> &list);

// utils

std::string long_to_str(long nb);
std::string &str_tolower(std::string &s);
void print_request(HttpRequest &request);
std::string &remove_trailing_spaces(std::string &s);
bool str_is_number(std::string &str);

#endif
