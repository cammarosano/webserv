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

// replaces RH by an ErrorRH or disconnects client if already an ErrorRH
void handle_response_time_out(Client &client, FdManager &table)
{
	int time_out_code;

	time_out_code = client.request_handler->time_out_code();
	if (!time_out_code) // terminate response and disconnect client
		remove_client(client, table, "webserv (response time-out)");
	else
		replace_req_handler(client, time_out_code, table);
}

void time_out_requests(FdManager &table, time_t now)
{
    // iterate over clients with incoming requests
    std::list<Client*> &list = Client::incoming_req_clients;
    std::list<Client*>::iterator it = list.begin();

    while (it != list.end())
	{
        Client &client = **it;
        ++it; // move iterator, as following operations might invalidate it
		if (difftime(now, client.time_begin_request) > REQUEST_TIME_OUT)
			send_error_resp_no_request(client, table, 408);
	}
}

void time_out_responses(FdManager &table, time_t now)
{
	// iterate over clients with ongoing responses
    std::list<Client*> &list = Client::ongoing_resp_clients;
    std::list<Client*>::iterator it = list.begin();

    while (it != list.end())
	{
        Client &client = **it;
        ++it; // move iterator, as following operations might invalidate it
		if (difftime(now, client.last_io) > RESPONSE_TIME_OUT)
			handle_response_time_out(client, table);
	}
}

void time_out_idle_clients(FdManager &table, time_t now)
{
	// iterate over idle clients
    std::list<Client*> &list = Client::idle_clients;
    std::list<Client*>::iterator it = list.begin();

    while (it != list.end())
	{
        Client &client = **it;
        ++it; // move iterator, as following operations might invalidate it
		if (difftime(now, client.last_io) > CONNECTION_TIME_OUT
			|| Client::counter > N_CLIENTS_CLEANUP)
			remove_client(client, table, "webserv (connection time-out)");
	}
}

void reaper(FdManager &table)
{
	static time_t last_run = time(NULL);
	time_t now = time(NULL);

	if (difftime(now, last_run) < REAPER_FREQUENCY)
		return;
	last_run = now;

	time_out_requests(table, now);
	time_out_responses(table, now);
	time_out_idle_clients(table, now);
	reap_child_processes(ACgiRH::child_processes);
}
