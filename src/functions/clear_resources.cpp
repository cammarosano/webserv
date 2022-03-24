#include "includes.hpp"

void clear_resources(FdManager &table, std::list<AReqHandler*> &list)
{
	typedef std::list<AReqHandler*>::iterator iterator;

	// clear RequestHandlers
	for (iterator it = list.begin(); it != list.end(); ++it)
		clear_rh(*it);
	// wait for child processes
	while (table.reap_child_processes() == 0) ;
	
	for (int fd = 0; fd < table.len(); fd++)
	{
		// disconnect clients
		if (table[fd].type == fd_client_socket)
			disconnect_client(*table[fd].client, table, "webserv(shut-down)");
		// close file descriptors
		else if (table[fd].type != fd_none)
			close(fd);
	}
}

void clear_rh(AReqHandler *req_handler)
{
	req_handler->unlock_client();
	delete req_handler->getRequest();
	delete req_handler;
}

// close socket, delete Client object and remove from table
// (if ongoing response) send abort signal to Request Handler
void disconnect_client(Client &client, FdManager &table,
                        const char *who)
{
    // log
    std::cout << "Connection closed by " << who << ": " << client.ipv4_addr
            << " (" << client.host_name << ")" << std::endl;

    close(client.socket);
    if (client.ongoing_response)
    {
        client.ongoing_response->disconnect_client();
        client.ongoing_response->abort();
    }
    table.remove_fd(client.socket);
    delete &client;
}
