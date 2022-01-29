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

std::map<int, fd_info> & Fd_table::getFd_map()
{
	return (fd_map);
}

void Fd_table::add_listening_socket(int listening_socket)
{
	fd_map[listening_socket].type = fd_listening_socket;
	poll_array.tag_for_addition(listening_socket);
}

void Fd_table::add_client(int client_socket)
{
	fd_map[client_socket].client = new Client(client_socket);
	fd_map[client_socket].type = fd_client_socket;
	poll_array.tag_for_addition(client_socket);
}

void Fd_table::add_fd_file(int fd, Client &client, HttpResponse &response)
{
	fd_map[fd].type = fd_file;
	fd_map[fd].client = &client;
	fd_map[fd].response = &response;

	poll_array.tag_for_addition(fd);
}

void Fd_table::remove_fd_read(int fd)
{
	fd_map.erase(fd);
	poll_array.tag_for_removal_fd(fd);
}

void Fd_table::remove_client(int client_socket)
{
	// look into each queued Response for ressources (fd file, cgi process)
	// and close them.
	Client &client = *fd_map[client_socket].client;
	while (!client.response_q.empty())
	{
		HttpResponse &response = client.response_q.front();
		if (response.source_type == file)
		{
			close(response.fd_read);
			remove_fd_read(response.fd_read); // remove from this table
		}
		// TODO: cgi stuff

		client.response_q.pop();
	}

	close(client_socket);
	delete fd_map[client_socket].client; // free memory
	fd_map.erase(client_socket);
	poll_array.tag_for_removal_fd(client_socket);
}

fd_info & Fd_table::operator[](int fd)
{
	return (fd_map[fd]);
}

// only clients with unsent data will be |= POLLOUT
void Fd_table::update_clients_out()
{
	struct pollfd * array = poll_array.getArray();
	int len = poll_array.getLen();

	for (int i = 0; i < len; i++)
	{
		fd_info info = fd_map[array[i].fd];
		if (info.type == fd_client_socket)
		{
			if (info.client->unsent_data.empty())
				array[i].events &= ~POLLOUT;
			else
				array[i].events |= POLLOUT;
		}
	}
}
