#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "config.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <iostream>
# include <cstdio> // perror
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
    enum e_state {idle, incoming_request, ongoing_response};

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

	// time-outs 
    time_t time_begin_request;
    time_t last_io; // send2client || new_requests()

    // constructor
    Client(int socket, sockaddr sa, std::list<Vserver> &vservers);
    // destructor
    ~Client();

	// state checks and changes
    void update_state();
    void update_state(e_state new_state);
    bool is_idle();
    bool is_incoming_request();
    bool is_ongoing_response();

    // lists 
    std::list<Client*>::iterator list_node;

    static std::list<Client*> idle_clients;
    static std::list<Client*> incoming_req_clients;
    static std::list<Client*> ongoing_resp_clients;

    // instances counter
    static int counter;

private:
    e_state state;

    // get ip address and host name
    void get_client_info(sockaddr &sa);
};


#endif