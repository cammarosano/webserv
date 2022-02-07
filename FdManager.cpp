#include "FdManager.hpp"
#include "ARequestHandler.hpp"

FdManager::FdManager(): capacity(10)
{
	fd_table = new fd_info[capacity];
	poll_array = new pollfd[capacity];

	// initialise poll_array
	for (int i = 0; i < capacity; i++)
		poll_array[i].fd = -1;
}

FdManager::~FdManager()
{
}

void FdManager::reallocate()
{
	size_t new_capacity = 2 * capacity;
	
	// get new poll_array
	pollfd *new_poll_array = new pollfd[new_capacity];
	// copy old stuff
	for (int i = 0; i < capacity; i++)
		new_poll_array[i] = poll_array[i];
	// free the old one
	delete[] poll_array;
	// initialize new stuff
	for (size_t i = capacity; i < new_capacity; i++)
		new_poll_array[i].fd = -1;

	// get new fd_table (initialization already done by default constructor)
	fd_info *new_fd_table = new fd_info[new_capacity];
	// copy old stuff
	for (int i = 0; i < capacity; i++)
		new_fd_table[i] = fd_table[i];
	// free the old one
	delete[] fd_table;
	
	// update member variables
	capacity = new_capacity;
	poll_array = new_poll_array;
	fd_table = new_fd_table;
}

// returns maximum fd + 1
// use this as upper bound for looping through the poll array of fd_table
int	FdManager::len() const
{
	int max_fd = *fd_set.rbegin();
	return max_fd + 1;
}

pollfd * FdManager::get_poll_array()
{
	return (poll_array);
}

void FdManager::add_listen_socket(int listen_socket,
									std::list<Vserver> &vservers)
{
	if (listen_socket >= capacity)
		reallocate();

	fd_table[listen_socket].type = fd_listen_socket;

	poll_array[listen_socket].fd = listen_socket;
	poll_array[listen_socket].events = POLLIN;
	poll_array[listen_socket].revents = 0;

	vservers_map[listen_socket] = vservers;

	fd_set.insert(listen_socket);
}

void FdManager::add_client_socket(int client_socket, Client &client)
{
	if (client_socket >= capacity)
		reallocate();
	
	fd_table[client_socket].type = fd_client_socket;
	fd_table[client_socket].client = &client;

	poll_array[client_socket].fd = client_socket;
	poll_array[client_socket].events = POLLIN;
	poll_array[client_socket].revents = 0;

	fd_set.insert(client_socket);
}

void FdManager::add_file_fd(int file_fd, Client &client)
{
	while (file_fd >= capacity)
		reallocate();
	
	fd_table[file_fd].type = fd_file;
	fd_table[file_fd].client = &client;
	fd_table[file_fd].is_EOF = false;

	poll_array[file_fd].fd = file_fd;
	poll_array[file_fd].events = POLLIN;
	poll_array[file_fd].revents = 0;

	fd_set.insert(file_fd);
}

void FdManager::remove_fd(int fd)
{
	// todo: request handler or do_io must close de fd

	fd_table[fd].type = fd_none;
	fd_table[fd].client = NULL;
	fd_table[fd].is_EOF = false;

	poll_array[fd].fd = -1;

	fd_set.erase(fd);
}


fd_info & FdManager::operator[](int fd)
{
	return (fd_table[fd]);
}

void	FdManager::set_pollout(int fd)
{
	poll_array[fd].events |= POLLOUT;
}

void	FdManager::unset_pollout(int fd)
{
	poll_array[fd].events &= ~POLLOUT;
}

std::list<Vserver> & FdManager::get_vserver_lst(int listen_socket)
{
	return vservers_map[listen_socket];
}