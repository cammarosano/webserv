#include "ARequestHandler.hpp"

AReqHandler::AReqHandler(HttpRequest *request, FdManager &table)
    : request(request),
      client(request->client),
      table(table),
      keep_alive(true)
{
    // keep-alive false if server is busy
    if (Client::counter > MAX_CLIENTS * 0.9)
    {
        keep_alive = false;
        response.header_fields["Connection"] = "close";
    }
}

AReqHandler::~AReqHandler() {}




bool AReqHandler::response100_expected() {
    std::map<std::string, std::string>::iterator it =
        request->header_fields.find("expect");

    if (it != request->header_fields.end() &&
        str_tolower(it->second) == "100-continue")
        return (true);
    return (false);
}

// returns error_code for time-out response
int AReqHandler::time_out_code()
{
    return (408); // in doubt, blame it on the client
}


// transfer the content of a string to client.unsent_data (ex: http header, or
// an auto-generated html page)
// Returns 1 if complete, 0 otherwise
int AReqHandler::send_str(std::string &str) {
    int max_bytes;

    max_bytes = BUFFER_SIZE - client.unsent_data.size();
    if (max_bytes <= 0) return 0;

    client.unsent_data += str.substr(0, max_bytes);
    str.erase(0, max_bytes);
    table.set_pollout(client.socket);
    if (str.empty()) return 1;
    return 0;
}
