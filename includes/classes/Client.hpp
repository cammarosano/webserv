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

// forward declaration
class AReqHandler;

struct Client {
    int socket;
    std::list<Vserver> &vservers;
    std::string ipv4_addr;
    std::string host_name;

    bool rh_locked; // former "state". true means locked by Request Handler
    bool disconnect_after_send;

    // buffers
    std::string received_data;
    std::string unsent_data;
    std::string decoded_body;

    // ongoing response
    AReqHandler *ongoing_response;

	// time-out
    bool incoming_request;
    time_t time_begin_request;

    // constructor
    Client(int socket, sockaddr sa, std::list<Vserver> &vservers);

	private:
    // get ip address and host name
    void get_client_info(sockaddr &sa);
};


#endif