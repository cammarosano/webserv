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

	// add to list
    state = idle;
    idle_clients.push_back(this);
    list_node = --idle_clients.end();

    // time-out monitoring
    last_io = time(NULL);

    ++counter;
}

Client::~Client()
{
    close(socket);
    if (state == idle)
        idle_clients.erase(list_node);
    else if (state == incoming_request)
        incoming_req_clients.erase(list_node);
    else
    {
        ongoing_resp_clients.erase(list_node);
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
        idle_clients.erase(list_node);
    else if (state == incoming_request)
        incoming_req_clients.erase(list_node);
    else
        ongoing_resp_clients.erase(list_node);

    // add to new state set
    if (new_state == idle)
    {
        idle_clients.push_back(this);
        list_node = --idle_clients.end();
    }
    else if (new_state == incoming_request)
    {
        incoming_req_clients.push_back(this);
        list_node = --incoming_req_clients.end();
    }
    else 
    {
        ongoing_resp_clients.push_back(this);
        list_node = --ongoing_resp_clients.end();
    }
    
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

bool Client::is_idle()
{
    if (state == idle)
        return (true);
    return (false);
}

bool Client::is_ongoing_response()
{
    if (state == ongoing_response)
        return (true);
    return (false);
}

// static variables
std::list<Client*> Client::idle_clients;
std::list<Client*> Client::incoming_req_clients;
std::list<Client*> Client::ongoing_resp_clients;
int Client::counter = 0;
