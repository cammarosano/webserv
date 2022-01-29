#include "Fd_table.hpp"

Fd_table::Fd_table()
{
}

Fd_table::~Fd_table()
{
}

Poll_array & Fd_table::getPollArray()
{
	return (poll_array);
}

void Fd_table::add_listening_socket(int listening_socket)
{
	fd_map[listening_socket].type = fd_listening_socket;
	poll_array.tag_for_addition(listening_socket);
}

void Fd_table::add_client(int client_socket)
{
	fd_map[client_socket].client = new Client();
	fd_map[client_socket].type = fd_client_socket;
	poll_array.tag_for_addition(client_socket);
}

void Fd_table::add_fd_file(int fd, Client *client, HttpResponse *response)
{
	fd_map[fd].type = fd_file;
	fd_map[fd].client = client;
	fd_map[fd].response = response;

	poll_array.tag_for_addition(fd);
}

void Fd_table::remove_fd_read(int fd)
{
	fd_map.erase(fd);
	poll_array.tag_for_removal_fd(fd);
}

void Fd_table::remove_client(int client_socket)
{
	// TODO:
	// look into each queued Response for ressources (fd file, cgi process)
	// and close them.

	delete fd_map[client_socket].client;
	fd_map.erase(client_socket);
	close(client_socket);
	poll_array.tag_for_removal_fd(client_socket);
}

fd_info & Fd_table::operator[](int fd)
{
	return (fd_map[fd]);
}