#include "Client.hpp"

Client::Client(int socket, sockaddr sa, std::list<Vserver> &vservers):
socket(socket),
vservers(vservers),
disconnect_after_send(false),
request_handler(NULL)
{
	get_client_info(sa);
	// log
	std::cout << "Connection accepted: " << ipv4_addr << " (" <<
		host_name << ")" << std::endl;

    // planned capacity for buffers
    received_data.reserve(BUFFER_SIZE);
    unsent_data.reserve(BUFFER_SIZE);

    state = idle;
    idle_clients.insert(this);

    // time-out monitoring
    last_io = time(NULL);

    ++counter;
}

Client::~Client()
{
    close(socket);
    if (state == idle)
        idle_clients.erase(this);
    else if (state == incoming_request)
        incoming_req_clients.erase(this);
    else
    {
        ongoing_resp_clients.erase(this);
        // clear request and RH
        delete request;
        delete request_handler;
    }

    --counter;
}

void Client::get_client_info(sockaddr &sa)
{
	sockaddr_in * sa_in = reinterpret_cast<sockaddr_in *>(&sa);
    char ipv4[INET_ADDRSTRLEN] = {0};
    char host[50] = {0};

    if (inet_ntop(AF_INET, &(sa_in->sin_addr), ipv4, INET_ADDRSTRLEN) == NULL)
        perror("inet_ntop");
    if (getnameinfo(&sa, sizeof(sa), host, 50, NULL, 0, 0) == -1)
        perror("getnameinfo()");
    ipv4_addr = ipv4;
    host_name = host;
}

void Client::update_state(e_state new_state)
{
    if (state == new_state)
        return;

    // remove from old state set
    if (state == idle)
        idle_clients.erase(this);
    else if (state == incoming_request)
        incoming_req_clients.erase(this);
    else
        ongoing_resp_clients.erase(this);

    // add to new state set
    if (new_state == idle)
        idle_clients.insert(this);
    else if (new_state == incoming_request)
        incoming_req_clients.insert(this);
    else 
        ongoing_resp_clients.insert(this);
    
    // update state
    state = new_state;
}

// changes state automatically:
// idle -> incoming_resquest
// incoming_request -> ongoing_response
// ongoing_response -> idle (if received_data is empty) || incoming_request
void Client::update_state()
{
    if (state == idle)
        update_state(incoming_request);
    else if (state == incoming_request)
        update_state(ongoing_response);
    else
    {
        if (received_data.empty())
            update_state(idle);
        else
            update_state(incoming_request);
    }
}

// static variables
std::set<Client*> Client::idle_clients;
std::set<Client*> Client::incoming_req_clients;
std::set<Client*> Client::ongoing_resp_clients;
int Client::counter = 0;
