#include "includes.hpp"

// returns the fd of the listening socket, -1 if error
int get_listening_socket(std::string host_IP, unsigned short port)
{
	int listen_socket;

	// socket() syscall returns a file descriptor
	listen_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_socket == -1)
	{
		perror("socket");
		return (-1);
	}

	// enable socket to reuse address (avoid "Address already in use" error)
	int yes = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
				   sizeof(yes)) == -1)
	{
		perror("setsockopt");
		return (-1);
	}

	// make fd non-blocking (fcntl)
	if (fcntl(listen_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return (-1);
	}

	// set up address (host_ip:port) for the bind() call
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(host_IP.c_str());

	// bind a name (specific address) to socket
	if (bind(listen_socket, reinterpret_cast<struct sockaddr *>(&address),
			 sizeof(address)) == -1)
	{
		std::cerr << "Error: Could not bind to " << host_IP << ":" << port
				  << std::endl;
		perror("bind");
		return (-1);
	}

	// finally, make it a listening socket
	if (listen(listen_socket, QUEUE_MAX_SIZE) == -1)
	{
		perror("listen");
		return (-1);
	}

	// output a confimation message to the terminal
	std::cout << "Listening on " << host_IP << ":" << port << "..."
			  << std::endl;

	return (listen_socket);
}
