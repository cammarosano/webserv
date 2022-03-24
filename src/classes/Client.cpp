#include "Client.hpp"

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


Client::Client(int socket, sockaddr sa, std::list<Vserver> &vservers):
socket(socket),
vservers(vservers),
disconnect_after_send(false),
ongoing_response(NULL),
begin_request(false)
{
	get_client_info(sa);
	// log
	std::cout << "Connection accepted: " << ipv4_addr << " (" <<
		host_name << ")" << std::endl;
    // planned capacity for buffers
    received_data.reserve(BUFFER_SIZE);
    unsent_data.reserve(BUFFER_SIZE);
    // time-out monitoring
    last_io = time(NULL);
    inactive_clients.push(std::make_pair(last_io, socket));
}

// static variable
std::queue<std::pair<time_t, int> > Client::inactive_clients;
