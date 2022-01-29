#ifndef FD_TABLE_HPP
# define FD_TABLE_HPP

# include "includes.hpp"

class Fd_table
{
private:
	std::map<int, fd_info>	fd_map;
	Poll_array 				poll_array;

public:
	Fd_table();
	~Fd_table();

	Poll_array & getPollArray();
	std::map<int, fd_info> & getFd_map();
	void add_listening_socket(int listening_socket);
	void add_client(int client_socket);
	void add_fd_file(int fd, Client &client, HttpResponse &response);
	void remove_fd_read(int fd);
	void remove_client(int client_socket); 

	void update_clients_out();

	

	fd_info & operator[](int fd);
};


#endif