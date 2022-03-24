#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "config.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <iostream>
# include <stdio.h> // perror
# include <ctime>
# include "macros.h"
# include <queue>

// forward declaration
class AReqHandler;

struct Client {
    int socket;
    std::list<Vserver> &vservers;
    std::string ipv4_addr;
    std::string host_name;

    bool disconnect_after_send;

    // buffers
    std::string received_data;
    std::string unsent_data;
    std::string decoded_body;

    // ongoing response (NULL if no response in course)
    AReqHandler *ongoing_response;

	// time-out request
    bool begin_request;
    time_t time_begin_request;

    // time-out inactive client
    time_t last_io; // monitoring just send2client
    static std::queue<std::pair<time_t, int> > inactive_clients;

    // constructor
    Client(int socket, sockaddr sa, std::list<Vserver> &vservers);


	private:
    // get ip address and host name
    void get_client_info(sockaddr &sa);
};


#endif