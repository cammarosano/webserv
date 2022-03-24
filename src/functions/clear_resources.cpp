#include "includes.hpp"

void clear_resources(FdManager &table, std::list<AReqHandler*> &list)
{
	typedef std::list<AReqHandler*>::iterator iterator;

	for (int fd = 0; fd < table.len(); fd++)
	{
		// disconnect clients
		if (table[fd].type == fd_client_socket)
			clear_client(*table[fd].client, table, "webserv(shut-down)");
		// close file descriptors
		else if (table[fd].type != fd_none)
			close(fd);
	}
	// clear RequestHandlers
	for (iterator it = list.begin(); it != list.end(); ++it)
		clear_rh(*it);
	// wait for child processes
	while (table.reap_child_processes() == 0) ;
}

void clear_rh(AReqHandler *req_handler)
{
	delete req_handler->getRequest();
	delete req_handler;
}

// close socket, delete Client object and remove from table
// (if ongoing response) send abort signal to Request Handler
void clear_client(Client &client, FdManager &table,
                        const char *who)
{
    // log
    std::cout << "Connection closed by " << who << ": " << client.ipv4_addr
            << " (" << client.host_name << ")" << std::endl;

    if (client.ongoing_response)
        client.ongoing_response->abort();
    table.remove_fd(client.socket);
    close(client.socket);
    delete &client;
}
