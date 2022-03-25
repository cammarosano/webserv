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
# include <set>
# include <unistd.h>
# include "ARequestHandler.hpp"

// forward declaration
class AReqHandler;
struct HttpRequest;

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
    HttpRequest *request;
    AReqHandler *request_handler;

	// time-out request
    time_t time_begin_request;

    // time-out inactive client
    time_t last_io; // send2client || init_response

    // constructor
    Client(int socket, sockaddr sa, std::list<Vserver> &vservers);
    // destructor
    ~Client();

	// state
    enum e_state {idle, incoming_request, ongoing_response} state;
    void update_state();
    void update_state(e_state new_state);

    // sets 
    static std::set<Client*> idle_clients;
    static std::set<Client*> incoming_req_clients;
    static std::set<Client*> ongoing_resp_clients;

    // instances counter
    static int counter;

	private:
    // get ip address and host name
    void get_client_info(sockaddr &sa);
};


#endif