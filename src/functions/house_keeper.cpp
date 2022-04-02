#include "includes.hpp"

// waits for children processes without blocking
// SIGKILLs if child process has not exited yet
// returns 1 if list is empty, 0 otherwise
int reap_child_processes(std::list<pid_t> &list)
{
	std::list<pid_t>::iterator it = list.begin();

	while (it != list.end())
	{
		if (waitpid(*it, NULL, WNOHANG))
			it = list.erase(it);
		else
		{
			kill(*it, SIGKILL);
			++it;
		}
	}
	if (list.empty())
		return (1);
	return (0);
}

void timeout_connection(Client &client, FdManager &table)
{
	remove_client(client, table, "webserv (connection time-out)");
}

void timeout_request(Client &client, FdManager &table)
{
	send_error_resp_no_request(client, table, 408);
}

// replaces RH by an ErrorRH or disconnects client if already an ErrorRH
void timeout_response(Client &client, FdManager &table)
{
	int time_out_code;

	time_out_code = client.request_handler->time_out_code();
	if (!time_out_code) // terminate response and disconnect client
		remove_client(client, table, "webserv (response time-out)");
	else
	{
		replace_req_handler(client, time_out_code, table);
		// reset timeout timer and send to back of the list
		client.update_state(Client::ongoing_response);
	}
}

void time_out(FdManager &table, time_t now, std::list<Client*> &list,
		int threshold, void (*handler)(Client&, FdManager&))
{
	// oldest ones are at the front of the list
    std::list<Client*>::iterator it = list.begin();
	while (it != list.end())
	{
        Client &client = **it;
        ++it; // move iterator, as following operations might invalidate it
		if (difftime(now, client.last_state_change) > threshold)
			handler(client, table);
		else
			return; // stop when a non-timed-out client is found
	}
}

// checks for time-outs and handle them
// waits for child processes from terminated CGI responses
void house_keeper(FdManager &table)
{
	time_t now = time(NULL);

	time_out(table, now, Client::idle_clients, CONNECTION_TIMEOUT,
				timeout_connection);
	time_out(table, now, Client::incoming_req_clients, REQUEST_TIMEOUT,
				timeout_request);
	time_out(table, now, Client::ongoing_resp_clients, RESPONSE_TIMEOUT,
				timeout_response);
	reap_child_processes(ACgiRH::child_processes);
}
