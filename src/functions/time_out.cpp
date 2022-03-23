#include "includes.hpp"

bool is_request_timeout(Client &client)
{
	if (std::difftime(time(NULL), client.time_begin_request) > REQUEST_TIME_OUT)
		return (true);
	return (false);
}

void send_error_resp_no_request(Client &client, FdManager &table, int error_code)
{
	std::string body = ErrorRH::generate_error_page(error_code);
	std::string response = "HTTP/1.1 "
		+ long_to_str(error_code) + ' '
		+ ErrorRH::reason_phrases[error_code] + "\r\n"
		+ "Content-Length: "
		+ long_to_str(body.size())
		+ "\r\n\r\n" + body;
	client.unsent_data.append(response);
	table.set_pollout(client.socket);
	client.received_data.clear();
	table.unset_pollin(client.socket);
	client.disconnect_after_send = true;
}
