#include "includes.hpp"
#include "FdManager.hpp"

int do_io(FdManager &table)
{
	int	poll_ret;

	// debug
	std::cout << "Blocking at poll()" << std::endl;

	// call poll()
	poll_ret = poll(table.get_poll_array(), table.len(), -1);

	// debug
	std::cout << "poll() returned " << poll_ret << std::endl;

	if (poll_ret == -1)
	{
		perror("poll");
		return (-1);
	}

	for (int fd = 3; fd < table.len(); fd++)
	{
		if (table.get_poll_array()[fd].revents & POLLIN) // fd ready for reading
		{
			e_fd_type fd_type = table[fd].type;

			if (fd_type == fd_listen_socket)
				accept_connection(fd, table);
			else if (fd_type == fd_client_socket)
				recv_from_client(fd, table);
			else if (fd_type == fd_file)
				read_from_file(fd, table);
			// TODO: cgi_out
		}
		if (table.get_poll_array()[fd].revents & POLLOUT) // fd ready for writing
		{
			e_fd_type fd_type = table[fd].type;

			if (fd_type == fd_client_socket)
				send_to_client(fd, table);
			// TODO: cgi_in
		}
	}
	return (0);
}

int main(void)
{
	FdManager				table;
	std::queue<HttpRequest>	requests_queue;

	setup(table);
	while (1)
	{
		do_io(table);
		// raw-data -> request header (Resquest object) or payload (processed_data)
		process_incoming_data(table, requests_queue);
		handle_requests(requests_queue); // creates responses
		handle_responses(table);
	}

	return (0);
}
