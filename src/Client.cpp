#include "includes.hpp"

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
state(recv_header),
ongoing_response(NULL)
{
	get_client_info(sa);
	// log
	std::cout << "Connection accepted: " << ipv4_addr << " (" <<
		host_name << ")" << std::endl;
}
