#include "includes.hpp"

void clear_resources(FdManager &table)
{
	for (int fd = 0; fd < table.len(); fd++)
	{
		// clear clients
		if (table[fd].type == fd_client_socket)
			remove_client(*table[fd].client, table, "webserv(shut-down)");
		// clear listening sockets
		else if (table[fd].type == fd_listen_socket)
			close(fd);
	}
	// wait for child processes
	while (reap_child_processes(ACgiRH::child_processes) == 0) ;
}

// delete Client object and remove from table
void remove_client(Client &client, FdManager &table,
                        const char *who)
{
    // log
    std::cout << "Connection closed by " << who << ": " << client.ipv4_addr
            << " (" << client.host_name << ")" << std::endl;

    table.remove_fd(client.socket);
    delete &client;
}
