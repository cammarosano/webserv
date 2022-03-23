#include "includes.hpp"

bool is_request_timeout(Client &client)
{
	if (std::difftime(time(NULL), client.time_begin_request) > REQUEST_TIME_OUT)
		return (true);
	return (false);
}

void send_time_out_response(Client &client, FdManager &table)
{
	std::string body = ErrorRH::generate_error_page(408);
	std::string response = "HTTP/1.1 408 Request Timeout\r\n"
						"Content-Length: "
						+ long_to_str(body.size())
						+ "\r\n\r\n" + body;
	client.unsent_data.append(response);
	table.set_pollout(client.socket);
	client.received_data.clear();
	table.unset_pollin(client.socket);
	client.disconnect_after_send = true;
}
