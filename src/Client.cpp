#include "includes.hpp"

void Client::get_peer_info()
{
    struct sockaddr_in sa;
    socklen_t addrlen = sizeof(sa);
    char ipv4[INET_ADDRSTRLEN] = {0};
    char host[50] = {0};

    if (getpeername(socket, reinterpret_cast<sockaddr *>(&sa), &addrlen) == -1)
        perror("getpeername()");
    if (inet_ntop(AF_INET, &(sa.sin_addr), ipv4, INET_ADDRSTRLEN) == NULL)
        perror("inet_ntop");
    if (getnameinfo(reinterpret_cast<sockaddr *>(&sa),
        addrlen, host, 50, NULL, 0, 0) == -1)
        perror("getnameinfo()");
    ipv4_addr = ipv4;
    host_name = host;
}


Client::Client(int socket, std::list<Vserver> &vservers):
socket(socket),
vservers(vservers),
state(recv_header),
ongoing_response(NULL)
{
	get_peer_info();
	// log
	std::cout << "Connection accepted: " << ipv4_addr << " (" <<
		host_name << ")" << std::endl;
}
