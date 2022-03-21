#include "includes.hpp"

void replace_request_handler(std::list<ARequestHandler*>::iterator it,
							int error_code, FdManager &table)
{
	ARequestHandler *old_rh, *new_rh;

	old_rh = *it;
	old_rh->unlock_client();
	new_rh = new ErrorRH(old_rh->getRequest(), table, error_code); 
	new_rh->lock_client();
	delete old_rh;
	*it = new_rh;
}

void clear_rh(ARequestHandler *req_handler)
{
	req_handler->unlock_client();
	delete req_handler->getRequest();
	delete req_handler;
}

void send_time_out_response(ARequestHandler *req_handler, FdManager &table)
{
	Client &client = req_handler->getRequest()->client;
	std::string response(
            "HTTP/1.1 408 Request Timeout\r\n"
            "Content-Length: 54\r\n"
            "\r\n"
            "<html><body><h1>408 Request Timeout</h1></body></html>"
	);
	client.unsent_data.append(response);
	table.set_pollout(client.socket);
	client.disconnect_after_send = true;
}

// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
int handle_requests(std::list<ARequestHandler *> &list, FdManager &table)
{
    std::list<ARequestHandler *>::iterator it;
    ARequestHandler *req_handler;
    int ret;

    // iterate over list of request handlers
    it = list.begin();
    while (it != list.end())
    {
        req_handler = *it;
        ret = req_handler->respond();
		if (ret == 1 || ret == -1) // finished or aborted
		{
			clear_rh(req_handler);
			it = list.erase(it); // returns iterator to next elem of list
		}
		else if (ret > 1) // replace with error response
			replace_request_handler(it, ret, table); // iterator does not move
		else if (req_handler->is_time_out())
		{
			send_time_out_response(req_handler, table);
			req_handler->abort();
			clear_rh(req_handler);
			it = list.erase(it);
		}
		else // not finished (ret == 0)
			++it;
    }
	table.reap_child_processes();
    return (0);
}
