#include "includes.hpp"
#include "Fd_table.hpp"

void transfer_header_to_buffer(Client &client, HttpResponse &response)
{
	int max_bytes = BUFFER_SIZE - client.unsent_data.size();
	if (max_bytes <= 0) // buffer is full
		return ;
	client.unsent_data += response.header_str.substr(0, max_bytes);
	response.header_str.erase(0, max_bytes);
	if (response.header_str.empty())
	{
		if (response.source_type == none)
			response.state = done;
		else if (response.source_type == file)
			response.state = start_send_file;
	}
}

// handle the response at the front of the client's queue
int handle_front_response(Client &client, Fd_table &table)
{
	HttpResponse &response = client.response_q.front();

	if (response.state == sending_header)
		transfer_header_to_buffer(client, response);
	if (response.state == start_send_file)
	{
		table.add_fd_file(response.fd_read, client, response);
		response.state = sending_file;
	}
	// if state == sending_file, do nothing
	if (response.state == send_file_complete)
	{
		close(response.fd_read);
		table.remove_fd_read(response.fd_read);
		response.state = done;
	}
	if (response.state == done)
		client.response_q.pop();

	return (0);
}

// iterate over clients in the table and handle the oldest queued response
int handle_responses(Fd_table &table)
{
	typedef std::map<int, fd_info>::iterator map_iter;

	std::map<int, fd_info> &fd_map = table.getFd_map();

	for (map_iter it = fd_map.begin(); it != fd_map.end(); ++it)
	{
		if (it->second.type != fd_client_socket)
			continue;

		Client &client = *it->second.client;
		if (client.response_q.empty())
			continue;
		handle_front_response(client, table);
	}
	
	return (0);
}