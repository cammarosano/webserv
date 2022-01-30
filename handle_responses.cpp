#include "includes.hpp"
#include "FdManager.hpp"

void transfer_header_to_buffer(Client &client, HttpResponse &response,
								FdManager &table)
{
	int max_bytes = BUFFER_SIZE - client.unsent_data.size();
	if (max_bytes <= 0) // buffer is full
		return ;
	client.unsent_data += response.header_str.substr(0, max_bytes);
	response.header_str.erase(0, max_bytes);
	table.set_pollout(client.socket);
	if (response.header_str.empty())
	{
		if (response.source_type == none)
			response.state = done;
		else if (response.source_type == file)
			response.state = start_send_file;
	}
}

// handle the response at the front of the client's queue
int handle_front_response(Client &client, FdManager &table)
{
	HttpResponse &response = client.response_q.front();

	if (response.state == sending_header)
		transfer_header_to_buffer(client, response, table);
	if (response.state == start_send_file)
	{
		table.add_file_fd(response.fd_read, client, response);
		response.state = sending_file;
		return (0);
	}
	// if state == sending_file, do nothing
	if (response.state == send_file_complete)
	{
		table.remove_fd(response.fd_read);
		response.state = done;
	}
	if (response.state == done)
		client.response_q.pop();

	return (0);
}

// iterate over clients in the table and handle the oldest queued response
int handle_responses(FdManager &table)
{
	for (int fd = 3; fd < table.len(); ++fd)
	{
		if (table[fd].type != fd_client_socket)
			continue;

		Client &client = *table[fd].client;
		if (client.response_q.empty())
			continue;
		handle_front_response(client, table);
	}
	
	return (0);
}