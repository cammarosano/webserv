#include "includes.hpp"

// replace Client's request handler for an ErrorRH
void replace_req_handler(Client &client, int error_code, FdManager &table)
{
	delete client.request_handler;
	client.request_handler = new ErrorRH(client.request, table, error_code); 
	// client.disconnect_after_send = true; // this is wrong!!
	// make this standard behaviour for all error responses?
}

void finish_response(Client &client, FdManager &table)
{
	
	if (!client.unsent_data.empty())
	// RH's job is done, but client still hasn't received
	// the complete response
	{
		if (Client::counter > MAX_CLIENTS / 2)
			client.disconnect_after_send = true;
		return ; 
	}

	delete client.request;
	delete client.request_handler;
    // remove trailing spaces (possible left-overs from request's body)
    remove_trailing_spaces(client.received_data);
	client.update_state();

	// avoid blocking at poll, force send_to_client() to be called
	if (client.is_incoming_request() || client.disconnect_after_send)
		table.set_pollout(client.socket); 
}

// For each client in "ongoing_response" state, calls the respond() method of
// its request handler.
// Respond() return values are:
// 1 : finished
// 0 : not finished
// HTTP error_code: finished with error
//
// When response is finished:
// - deletes request and request handler
// - updates client's state
// When response() returns an error code, the request handler is replaced by
// an ErrorRH.
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
		if (ret == 1) // response is complete
			finish_response(client, table);
		else if (ret > 1)
			replace_req_handler(client, ret, table);
    }
}
