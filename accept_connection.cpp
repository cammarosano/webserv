#include "includes.hpp"

// accept(), add new Client to map<socket,Client>
int accept_connection(int listen_socket, std::map<int,Client> &m)
{
	int         client_socket;
	sockaddr    client_addr;
	socklen_t   addrlen = sizeof(client_addr);

    // accept new connection
    client_socket = accept(listen_socket, &client_addr, &addrlen);
    if (client_socket == -1)
    {
        perror("accept");
        return (-1);
    }

    // add to map of clients
	m[client_socket].recv_state = get_header;

	// log to terminal
    std::cout	<< "Connection accepted" << std::endl;
    
	return (client_socket);
}
