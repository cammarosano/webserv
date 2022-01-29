#include "includes.hpp"

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
			response.state == done;
		else if (response.source_type == file)
			response.state == start_send_file;
	}
}

int handle_response(int socket, Client &client, Poll_array &poll_array)
{
	HttpResponse &response = client.response_q.front();

	if (response.state == sending_header)
		transfer_header_to_buffer(client, response);
	if (response.state == start_send_file)
	{
		poll_array.tag_for_addition(response.fd_read);
		response.state =  sending_file;
	}
	if (response.state == done)
		client.response_q.pop();

}

int do_response_actions(std::map<int, Client> &clients, Poll_array &poll_array)
{
	typedef std::map<int, Client>::iterator iterator;
	
	for (iterator it = clients.begin(); it != clients.end(); ++it)
	{
		int client_socket = it->first;
		Client &client = it->second;



	}
}