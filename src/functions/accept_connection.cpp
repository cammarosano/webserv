#include "includes.hpp"

// returns 1 if OK, new client can be accepted
// returns 0 otherwise
int make_room_for_new_client(FdManager &table)
{
	time_t now = time(NULL);

	if (Client::idle_clients.empty())
		return (0);

	std::list<Client *>::iterator it = Client::idle_clients.begin();
	while (it != Client::idle_clients.end())
	{
		Client &oldest_idle_client = **it;
		if (table.get_poll_array()[oldest_idle_client.socket].revents)
			++it; // skip this one, as an incoming request is likely
		else if (difftime(now, oldest_idle_client.last_state_change) <
				 MIN_IDLE_TIME)
			return (0);
		else
		{
			remove_client(oldest_idle_client, table,
						  "webserv (max num clients reached)");
			return (1);
		}
	}
	return (0);
}

// accept() a new connection, create a Client object
void accept_connection(int listen_socket, FdManager &table)
{
	int client_socket;
	sockaddr client_addr;
	socklen_t addrlen = sizeof(client_addr);

	if (Client::counter >= MAX_CLIENTS)
	{
		// (void)now;
		if (!make_room_for_new_client(table))
			return; // make it wait for next IO round
	}

	// accept new connection
	client_socket = accept(listen_socket, &client_addr, &addrlen);
	if (client_socket == -1)
	{
		perror("accept");
		return;
	}

	// make sure it's a non-blocking socket (see OBS below)
	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return;
	}

	// create Client
	Client *client = new Client(client_socket, client_addr,
								table.get_vserver_lst(listen_socket));
	table.add_client_socket(client_socket, *client);

	// log to terminal
	if (DEBUG)
		std::cout << "Connection accepted. Client socket: " << client_socket
				  << std::endl;
}

/*
OBS: man page accept() linux
 On Linux, the new socket returned by accept() does not inherit file status
flags such as O_NONBLOCK and O_ASYNC from  the listening  socket.   This
behavior  differs from the canonical BSD sockets implementation.  Portable
programs should not rely on inheritance or noninheritance of file status flags
and always explicitly set all required flags on the socket re‐ turned from
accept().
*/
