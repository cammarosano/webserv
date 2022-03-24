#include "includes.hpp"

void replace_request_handler(std::list<AReqHandler*>::iterator it,
							int error_code, FdManager &table)
{
	AReqHandler *old_rh, *new_rh;

	old_rh = *it;
	old_rh->unlock_client();
	new_rh = new ErrorRH(old_rh->getRequest(), table, error_code); 
	new_rh->lock_client();
	delete old_rh;
	*it = new_rh;
	new_rh->getRequest()->client.disconnect_after_send = true;
	// make this standard behaviour for all error responses?
}

// replaces RH or disconnects client
void handle_time_out(std::list<AReqHandler *>::iterator &it,
	std::list<AReqHandler*> list, FdManager &table)
{
    AReqHandler *req_handler = *it;
	int time_out_code;

	time_out_code = req_handler->time_out_abort();
	if (!time_out_code) // terminate response and disconnect client
	{
		Client &client = req_handler->getRequest()->client;
		clear_rh(req_handler);
		disconnect_client(client, table, "webserv(time-out)");
		it = list.erase(it);
		return ;
	}
	replace_request_handler(it, time_out_code, table);
}
// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
int handle_requests(std::list<AReqHandler *> &list, FdManager &table)
{
    std::list<AReqHandler *>::iterator it;
    AReqHandler *req_handler;
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
			handle_time_out(it, list, table); // iterator might move
		else // not finished (ret == 0)
			++it;
    }
	table.reap_child_processes();
    return (0);
}
