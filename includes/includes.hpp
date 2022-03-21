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
#include <stdlib.h>  // atoi
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>

# include "macros.h"
# include "config.hpp"
# include "utils.h"

// classes
# include "Client.hpp"
# include "FdManager.hpp"
# include "ARequestHandler.hpp"
# include "HttpRequest.hpp"
# include "CgiGetRH.hpp"
# include "CgiPostRH.hpp"
# include "DirectoryRH.hpp"
# include "ErrorRH.hpp"
# include "RedirectRH.hpp"
# include "StaticRH.hpp"
# include "ConfigParser.hpp"

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
int handle_requests(std::list<ARequestHandler *> &list);

// clear

void disconnect_client(Client &client, FdManager &table, const char *who);

#endif
