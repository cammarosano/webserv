#include "includes.hpp"
#include "Poll_array.hpp"

int do_io(Poll_array &poll_array, std::map<int, Client> &clients_map, int listening_socket)
{
	int	poll_ret;

	poll_array.update();
	std::cout << "About to call poll(). Array size: "<< poll_array.getLen() << std::endl;
	poll_ret = poll(poll_array.getArray(), poll_array.getLen(), -1);
	if (poll_ret == -1)
	{
		perror("poll");
		return (-1);
	}
	std::cout << "poll() returned " << poll_ret << std::endl;
	for (int i = 0; i < poll_array.getLen(); i++)
	{
		if (poll_array[i].revents & POLLIN) // socket is ready for reading
		{
			int fd = poll_array[i].fd;
			std::cout << fd << " is ready for reading" << std::endl;

			if (fd == listening_socket) // request for new connection
			{
				int client_socket = accept_connection(fd, clients_map);
				if (client_socket == -1)
					return (-1);
				// add client to poll_array
				poll_array.tag_for_addition(client_socket);
			}
			else	// client is sending data
			{
				int ret = recv_from_client(fd, clients_map);
				if (ret == -1) // error
					return (-1);
				if (ret == 0) // connection was closed
					poll_array.tag_for_removal(i);
			}
		}
		if (poll_array[i].revents & POLLOUT) // socket is ready for writing
		{
			std::cout << poll_array[i].fd << " is ready for writing" << std::endl;
			int ret = send_to_client(poll_array[i].fd, clients_map);

			if (ret == -1) // error
				return (-1);
		}
	}
	return (0);
}

int main(void)
{
	int						listening_socket;
	Poll_array				poll_array;
	std::map<int, Client>	clients_map;

	// get listening socket and add it to poll_array
	listening_socket = get_listening_socket("127.0.0.1", 8080);
	if (listening_socket == -1)
		return (1);
	poll_array.tag_for_addition(listening_socket);

	while (1)
	{
		int ret = do_io(poll_array, clients_map, listening_socket);
		if (ret == -1)
			return (1);
		
		// process data
		// for client in clients, map, depending on the state, extract header or transfer data to somewhere else

		
				// if (ret == 1) // a request was received and processed
				// 	poll_array[i].events |= POLLOUT;

				// 	// removing a write_watch form poll
				// poll_array[i].events &= ~POLLOUT;
	}
	

	return (0);
}
