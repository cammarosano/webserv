#ifndef FDMANAGER_HPP
# define FDMANAGER_HPP

# include "includes.hpp"

enum e_fd_type
{
	fd_none,
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

	fd_info(): type(fd_none), client(NULL), response(NULL) {}
};

class FdManager
{
private:
	pollfd *		poll_array;
	fd_info *		fd_table;
	size_t			capacity;
	std::set<int>	fd_set;
	
	std::map<int, std::list<Vserver> >	vservers_map;

	void	reallocate();
	void	free_client_ressources(Client *client);

public:
	FdManager();
	~FdManager();
	
	size_t	len() const;
	pollfd	*get_poll_array();
	void 	add_listen_socket(int listen_socket, std::list<Vserver> &vservers);
	void 	add_client_socket(int client_socket, int listen_socket);
	void 	add_file_fd(int file_fd, Client &client, HttpResponse &response);
	void 	remove_fd(int fd);

	void	set_pollout(int fd);
	void	unset_pollout(int fd);

	fd_info & operator[](int fd);
};


#endif