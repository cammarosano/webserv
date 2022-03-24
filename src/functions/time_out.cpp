#include "includes.hpp"

void time_out_inactive_clients(FdManager &table)
{
	time_t last_io;
	time_t now = time(NULL);
	int fd;
    std::queue<std::pair<time_t, int> > &q = Client::inactive_clients;

	while (!q.empty())
	{
		last_io = q.front().first;
		fd = q.front().second;
		// Client has been deleted or last_io is not updated
		// (client might be duplicated in the queue)
		if (table[fd].type != fd_client_socket
			|| table[fd].client->last_io > last_io)
		{
			q.pop();
			continue;
		}
		if (difftime(now, last_io) < CLIENT_TIME_OUT)
			return ; // no one in the q is old enough to be timed-out
		clear_client(*table[fd].client, table,
						"webserv (connection time-out)");
		q.pop();
	}
}

void reaper(FdManager &table)
{
	// time-out requests
	// reap child processes: move reaper's list to ACgiRH
	time_out_inactive_clients(table);

}
