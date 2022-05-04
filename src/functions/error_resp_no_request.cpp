#include "includes.hpp"

// Sends an error response without an HttpRequest object
void send_error_resp_no_request(Client &client, FdManager &table,
								int error_code)
{
	// body
	std::string body = ErrorRH::generate_error_page(error_code);

	// header
	HttpResponse response;
	response.status_code = error_code;
	response.header_fields["Content-Length"] = long_to_str(body.size());
	response.header_fields["Content-Type"] = "text/html";
	response.header_fields["Connection"] = "close";
	response.assemble_header_str();

	client.unsent_data.append(response.header_str);
	client.unsent_data.append(body);
	table.set_pollout(client.socket);
	client.disconnect_after_send = true;

	client.received_data.clear();
	table.unset_pollin(client.socket);

	client.update_state(Client::idle);
	// shall be reaped by connection time-out if data is never sent
}
