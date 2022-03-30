#include "includes.hpp"

// replace Client's request handler for an ErrorRH
void replace_req_handler(Client &client, int error_code, FdManager &table)
{
	delete client.request_handler;
	client.request_handler = new ErrorRH(client.request, table, error_code); 
	client.disconnect_after_send = true;
	// make this standard behaviour for all error responses?
}

void finish_response(Client &client, FdManager &table)
{
	delete client.request;
	delete client.request_handler;
    // remove trailing spaces (possible left-overs from request's body)
    remove_trailing_spaces(client.received_data);
	client.update_state();
	if (Client::counter > MAX_CLIENTS / 2)
	{
		if (client.unsent_data.empty())
		{
			remove_client(client, table,
				"webserv (disconnect after response sent)");
			return; // experimental
		}
		else
			client.disconnect_after_send = true;
	}


}

// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
void handle_requests(FdManager &table)
{
	// iterate over clients with ongoing responses
    std::list<Client*>::iterator it;
    std::list<Client*> &list = Client::ongoing_resp_clients;
    AReqHandler *req_handler;
    int ret;

    // iterate over list of request handlers
    it = list.begin();
    while (it != list.end())
    {
		Client &client = **it;
		++it; // following operations might invalidate iterator
        req_handler = client.request_handler;
        ret = req_handler->respond();
		if (ret == 1)
			finish_response(client, table);
		else if (ret > 1)
			replace_req_handler(client, ret, table);
    }
}
