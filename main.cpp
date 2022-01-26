#include "includes.hpp"
#include "Poll_array.hpp"

// write the POLL loop for receiving requests (header only for now)

// write response to get request

int main(void)
{
	int						listening_socket;
	Poll_array				poll_array;
	int						poll_ret;
	std::map<int, Client>	clients_map;

	listening_socket = get_listening_socket("127.0.0.1", 8080);
	if (listening_socket == -1)
		return (1);
	
	std::cout << "Listening fd: " << listening_socket << std::endl;

	poll_array.tag_for_addition(listening_socket);

	while (1)
	{
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
				std::cout << poll_array[i].fd << " is ready for reading" << std::endl;
				if (poll_array[i].fd == listening_socket) // request for new connection
				{
					// accept() and add to clients_map
					int client_fd = accept_connection(poll_array[i].fd, clients_map);
					if (client_fd == -1)
						return (1);
					// add client to poll_array 
					poll_array.tag_for_addition(client_fd);
					std::cout << "client fd added to poll array: " << client_fd << std::endl;
				}

				else	// an active connection is sending data
				{
					int ret = handle_incoming_data(poll_array[i].fd, clients_map);
					if (ret == -1) // error
						return (1);
					if (ret == 0) // connection was closed
					{
						// remove client_fd from array and map
						poll_array.tag_for_removal(i);
						clients_map.erase(poll_array[i].fd);
					}
					if (ret == 1) // a request was received
					{
						poll_array[i].events |= POLLOUT;
					}
				}
			}
			if (poll_array[i].revents & POLLOUT) // socket is ready for writing
			{
				std::cout << poll_array[i].fd << " is ready for writing" << std::endl;
				handle_outbound_data(poll_array[i].fd);

				// no keep-alive for now
				close(poll_array[i].fd);
				poll_array.tag_for_removal(i);
				clients_map.erase(poll_array[i].fd);
			}

		}
		

	}
	

	return (0);
}
