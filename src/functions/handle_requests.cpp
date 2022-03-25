#include "includes.hpp"

// replace Client's request handler for an ErrorRH
void replace_request_handler(Client &client,
							int error_code, FdManager &table)
{
	delete client.request_handler;
	client.request_handler = new ErrorRH(client.request, table, error_code); 
	client.disconnect_after_send = true;
	// make this standard behaviour for all error responses?
}

// replaces RH or disconnects client
void handle_time_out(Client &client, FdManager &table)
{
    AReqHandler *req_handler = client.request_handler;
	int time_out_code;

	time_out_code = req_handler->time_out_code();
	if (!time_out_code) // terminate response and disconnect client
		remove_client(client, table, "webserv(time-out)");
	else
		replace_request_handler(client, time_out_code, table);
}

void finish_response(Client &client)
{
	delete client.request;
	delete client.request_handler;
	client.update_state();
}

// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
void handle_requests(FdManager &table)
{
	// iterate over clients with ongoing responses
    std::set<Client*>::iterator it;
    std::set<Client*> &set = Client::ongoing_resp_clients;
    AReqHandler *req_handler;
	time_t current_time = time(NULL);
    int ret;

    // iterate over list of request handlers
    it = set.begin();
    while (it != set.end())
    {
		Client &client = **it;
		++it; // following operations might invalidate iterator
        req_handler = client.request_handler;
        ret = req_handler->respond();
		if (ret == 1)
			finish_response(client);
		else if (ret > 1)
			replace_request_handler(client, ret, table);
		else if (req_handler->is_time_out(current_time))
			handle_time_out(client, table);
    }
}
