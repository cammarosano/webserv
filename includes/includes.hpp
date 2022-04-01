#ifndef INCLUDES_HPP
#define INCLUDES_HPP

// C stuff
# include <cstring> // memset
# include <csignal>

// classes
#include "ARequestHandler.hpp"
#include "CgiGetRH.hpp"
#include "CgiPostRH.hpp"
#include "Client.hpp"
#include "ConfigParser.hpp"
#include "DeleteRH.hpp"
#include "DirectoryRH.hpp"
#include "ErrorRH.hpp"
#include "FdManager.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "PostRH.hpp"
#include "RedirectRH.hpp"
#include "StaticRH.hpp"

// function prototypes

// setup

int setup(FdManager &table, int argc, char **argv);
int get_listening_socket(std::string host_IP, unsigned short port);
int parse_mime_types_file(std::map<std::string, std::string> &map);

// IO

void accept_connection(int listen_socket, FdManager &table, time_t now);
void recv_from_client(int socket, FdManager &table, time_t now);
void send_to_client(int socket, FdManager &table, time_t current_time);
void read_from_fd(int fd, FdManager &table);
void write_to_fd(int fd_cgi_input, FdManager &table);

// process requests

void new_requests(FdManager &table);
AReqHandler *init_response(HttpRequest &request, FdManager &table);
void handle_requests(FdManager &table);
void replace_req_handler(Client &client, int error_code, FdManager &table);

// clear

void remove_client(Client &client, FdManager &table, const char *who);
void clear_resources(FdManager &table);
int reap_child_processes(std::list<pid_t> &list);

// error-response without request

void send_error_resp_no_request(Client &client, FdManager &table,
								int error_code);

// time-out and wait CGI processes

void reaper(FdManager &table);

#endif
