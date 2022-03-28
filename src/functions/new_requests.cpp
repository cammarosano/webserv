#include "includes.hpp"

// extracts data from the client's received_data buffer into an HttpRequest
// object returns NULL if buffer does not contain a complete request header
HttpRequest *new_HttpRequest(Client &client, FdManager &table)
{
    // look for end-of-header delimiter: 2CRLF
    size_t pos = client.received_data.find("\r\n\r\n");
    if (pos == std::string::npos) // not found: header is incomplete
    {
        // check if header size above limit
        if (client.received_data.size() == BUFFER_SIZE)
            send_error_resp_no_request(client, table, 431);
        return (NULL);
    }

    // header is complete: consume data
    std::string header_str = client.received_data.substr(0, pos);
    client.received_data.erase(0, pos + 4);

    // debug
    if (DEBUG)
        std::cout << "---------\nThe following request header was received:\n"
             << header_str << "\n" << std::endl;

    // create HttpRequest object
    return new HttpRequest(client, header_str);
}


// checks each Client's received_data buffer for a HTTP request header,
// instantiates a new HttpRequest and a suitable request handler
void new_requests(FdManager &table)
{
    // iterate over clients with incoming requests
    std::list<Client*>::iterator it;
    std::list<Client*> &list = Client::incoming_req_clients;
    time_t now = time(NULL);

    it = list.begin();
    while (it != list.end())
    {
        Client &client = **it;
        ++it; // move iterator, as following operations might invalidate it
        HttpRequest *request = new_HttpRequest(client, table);
        if (!request) 
            continue;
        AReqHandler *req_handler;
        try
        {
            req_handler = init_response(*request, table);
        }
        catch (const std::exception &e)
        {
            req_handler = new ErrorRH(request, table, 500);
        }
        // Client owns request and request handler 
        // move lines below to AReqHandler's constructor?
        client.request = request;
        client.request_handler = req_handler;
        client.update_state();
        client.last_io = now;
    }
}
