#include "includes.hpp"

	// std::string hello_msg = 
	// 	"HTTP/1.1 200 OK\r\n"
	// 	"Content-Type: text/plain\r\n"
	// 	"Content-Length: 6\r\n\r\n"
	// 	"Hello!";
	// send(socket, hello_msg.c_str(), hello_msg.size(), 0);

# define SEND_BUF_SIZE 1024


// return 0 if read file has been completed
int load_data_from_ressource(HttpResponse &response)
{
	int max_read = SEND_BUF_SIZE - response.unsent_data.size();
	if (max_read <= 0) // unsent data is already too big
		return (0);

	char buffer[SEND_BUF_SIZE + 1];
	
	int ret = read(response.fd_read, buffer, max_read);
	if (ret == -1)
	{
		perror("read");
		return (-1);
	}
	buffer[ret] = '\0';
	response.unsent_data += buffer;
	response.bytes_left -= ret;
	if (response.bytes_left == 0)
	{
		close(response.fd_read);
		return (0);
	}
	return (1);
}

// returns 0 if response transmission is complete, 1 if incomplete, -1 if error
int handle_outbound_data(int socket, Client &client)
{
	HttpResponse &response = client.response_q.front();

	if (response.state == start)
	{
		assemble_header(response);

		if (response.status_code_phrase == "200 OK")
			response.state = sending_file;
		else
			response.state = data_ready;
	}

	if (response.state == sending_file)
	{
		int ret = load_data_from_ressource(response);
		if (ret == -1)
			return (-1);
		if (ret == 0)
			response.state = data_ready; 
	}
	
	int bytes_sent = send(socket, response.unsent_data.data(),
							response.unsent_data.size(), 0);	
	if (bytes_sent == -1)
	{
		perror("send"); // which is not allowed by the subject!
		return (-1);
	}
	response.unsent_data.erase(0, bytes_sent);
	if (response.state == data_ready && response.unsent_data.size() == 0)
	{
		client.response_q.pop();
		return (0);
	}
	return (1);

}

