#include "ARequestHandler.hpp"

ARequestHandler::ARequestHandler(HttpRequest *request, FdManager &table)
    : request(request), client(request->client), table(table),
    client_disconnected(false), bytes_sent(0)
{
    update_last_io_activ();
}

ARequestHandler::~ARequestHandler() {}

HttpRequest *ARequestHandler::getRequest() { return request; }

void ARequestHandler::assemble_header_str() {
    typedef std::map<std::string, std::string>::iterator iterator;

    // status-line
    header_str =
        response.http_version + ' ' + response.status_code_phrase + "\r\n";

    // header-fiels
    for (iterator it = response.header_fields.begin();
         it != response.header_fields.end(); ++it)
        header_str += it->first + ": " + it->second + "\r\n";

    // end header
    header_str += "\r\n";

    // log status-line
    std::cout << "Response: " << response.http_version << " "
        << response.status_code_phrase << std::endl;
}

// transfer header_str to Client's unsent_data buffer
// return 1 if complete, 0 if incomplete
int ARequestHandler::send_header() {
    Client &client = request->client;
    int max_bytes;

    max_bytes = BUFFER_SIZE - client.unsent_data.size();
    if (max_bytes <= 0)  // buffer is full
        return 0;
    client.unsent_data += header_str.substr(0, max_bytes);
    header_str.erase(0, max_bytes);
    table.set_pollout(client.socket);
    if (header_str.empty()) return 1;
    return 0;
}

void ARequestHandler::update_last_io_activ()
{
    std::time(&last_io_activity);
}

bool ARequestHandler::is_time_out()
{
    if (std::difftime(time(NULL), last_io_activity) > REQUEST_TIME_OUT)
        return (true);
    return (false);
}

// trying to unlock a disconnectedd client
// or one locked by a different RH instance has no effect
void ARequestHandler::unlock_client()
{
    if (client_disconnected)
        return;
    if (client.rh_locked && client.ongoing_response == this)
    {
        client.rh_locked = false;
        client.ongoing_response = NULL;
    }
}

// trying to lock a already locked client raises an exception
void ARequestHandler::lock_client()
{
    if (client.rh_locked)
        throw (std::exception());
    client.rh_locked = true;
    client.ongoing_response = this;
}

void ARequestHandler::disconnect_client()
{
    client_disconnected = true;
}

void ARequestHandler::add_to_bytes_sent(size_t n)
{
    bytes_sent += n;
}
