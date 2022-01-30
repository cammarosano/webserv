#include "includes.hpp"
#include "FdManager.hpp"

// accept(), create new Client, update fd_table and poll_array
// return  0 if ok, -1 if error
int accept_connection(int listen_socket, FdManager &table)
{
	int client_socket;
	sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);

	// accept new connection
	client_socket = accept(listen_socket, &client_addr, &addrlen);
	if (client_socket == -1)
	{
		perror("accept");
		return (-1);
	}

	// create Client
	table.add_client_socket(client_socket, listen_socket);

	// log to terminal
	std::cout << "Connection accepted. Client socket: "
			<< client_socket << std::endl;

	return (client_socket);
}
