#include "includes.hpp"

int do_io(Fd_table &table)
{
	int	poll_ret;
	Poll_array &poll_array = table.getPollArray();

	poll_array.update(); // process aditions and removals
	table.update_clients_out();
	poll_ret = poll(poll_array.getArray(), poll_array.getLen(), -1);
	if (poll_ret == -1)
	{
		perror("poll");
		return (-1);
	}
	for (int i = 0; i < poll_array.getLen(); i++)
	{
		if (poll_array[i].revents & POLLIN) // socket is ready for reading
		{
			int fd = poll_array[i].fd;
			e_fd_type fd_type = table[fd].type;

			if (fd_type == fd_listening_socket)
				accept_connection(fd, table);
			else if (fd_type == fd_client_socket)
				recv_from_client(fd, table);
			else if (fd_type == fd_file)
				recv_from_file(fd, table);
			// TODO: cgi_out
		}
		if (poll_array[i].revents & POLLOUT) // socket is ready for writing
		{
			int fd = poll_array[i].fd;
			e_fd_type fd_type = table[fd].type;

			if (fd_type == fd_client_socket)
				send_to_client(poll_array[i].fd, table);
			// TODO: cgi_in
		}
	}
	return (0);
}

int main(void)
{
	int						listening_socket;
	Fd_table				table;
	std::queue<HttpRequest>	requests_queue;

	// get listening socket and add it to poll_array
	listening_socket = get_listening_socket("127.0.0.1", 8080);
	if (listening_socket == -1)
		return (1);
	table.add_listening_socket(listening_socket);

	while (1)
	{
		do_io(table);

		process_incoming_data(table, requests_queue);

		handle_requests(requests_queue, table); // creates responses

		handle_responses(table);
	}

	return (0);
}
