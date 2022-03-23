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

// IO

void accept_connection(int listen_socket, FdManager &table);
void recv_from_client(int socket, FdManager &table);
void read_from_fd(int fd, FdManager &table);
void send_to_client(int socket, FdManager &table);
void write_to_fd(int fd_cgi_input, FdManager &table);

// process requests

int new_requests(FdManager &table,
                 std::list<ARequestHandler *> &req_handlers_lst);
int handle_requests(std::list<ARequestHandler *> &list, FdManager &table);

// clear

void disconnect_client(Client &client, FdManager &table, const char *who);

#endif
