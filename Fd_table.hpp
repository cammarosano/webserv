#ifndef FD_TABLE_HPP
# define FD_TABLE_HPP

# include "includes.hpp"
# include "Poll_array.hpp"

enum e_fd_type
{
	fd_listen_socket,
	fd_client_socket,
	fd_file,
	fd_cgi_output,
	fd_cgi_input
};

struct fd_info
{
	e_fd_type		type;
	Client 			*client;
	HttpResponse	*response;

	fd_info(): client(NULL), response(NULL) {}
};

class Fd_table
{
private:
	std::map<int, fd_info>					fd_map;
	Poll_array 								poll_array;
	std::map<int, std::list<Vserver> >		vservers_map;

public:
	Fd_table();
	~Fd_table();

	Poll_array & getPollArray();
	std::map<int, fd_info> & getFd_map();

	void add_listening_socket(int listening_socket, std::list<Vserver> vservers);
	void add_client(int client_socket, int listening_socket);
	void add_fd_file(int fd, Client &client, HttpResponse &response);
	void remove_fd_read(int fd);
	void remove_client(int client_socket); 

	void update_clients_out();

	fd_info & operator[](int fd);
};


#endif