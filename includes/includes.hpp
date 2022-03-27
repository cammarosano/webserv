#ifndef INCLUDES_HPP
#define INCLUDES_HPP

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
#include <signal.h>
#include <stdlib.h>  // atoi
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>

#include "config.hpp"
#include "macros.h"
#include "utils.h"

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
#include "PostRH.hpp"
#include "RedirectRH.hpp"
#include "StaticRH.hpp"

// function prototypes

// setup

int setup(FdManager &table);
int get_listening_socket(std::string host_IP, unsigned short port);
int parse_mime_types_file(std::map<std::string, std::string> &map);

// IO

void accept_connection(int listen_socket, FdManager &table, time_t now);
void recv_from_client(int socket, FdManager &table, time_t now);
void read_from_fd(int fd, FdManager &table);
void send_to_client(int socket, FdManager &table, time_t current_time);
void write_to_fd(int fd_cgi_input, FdManager &table);

// process requests

void new_requests(FdManager &table);
AReqHandler *init_response(HttpRequest &request, FdManager &table);
void handle_requests(FdManager &table);

// clear

void remove_client(Client &client, FdManager &table, const char *who);
void replace_req_handler(Client &client, int error_code, FdManager &table);
void clear_resources(FdManager &table);
int reap_child_processes(std::list<pid_t> &list);

// error-response without request
void send_error_resp_no_request(Client &client, FdManager &table,
								int error_code);

// time-out
void reaper(FdManager &table);

#endif
