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
	bool			is_EOF;

	fd_info(): type(fd_none), client(NULL), is_EOF(false) {}
};

/*
Keeps track of all opened file descriptors, their role (ex: client socket,
listening socket, file in disk etc) and the Client to which it is
related.
A single instance of this class is used by the program.
It also maintains the array used by poll().
This class does not own any of these resources (Client objects or file 
descriptors). It does not acquire (new, open) nor releases (delete, close)
any of these resources.
*/
class FdManager
{
private:
	pollfd *		poll_array;
	fd_info *		fd_table;
	int				capacity;
	std::set<int>	fd_set;
	
	std::map<int, std::list<Vserver> >	vservers_map;

	void	reallocate();

public:
	FdManager();
	~FdManager();
	
	int		len() const;
	pollfd	*get_poll_array();
	void 	add_listen_socket(int listen_socket, std::list<Vserver> &vservers);
	void 	add_client_socket(int client_socket, Client &client);
	void 	add_file_fd(int file_fd, Client &client);
	void	add_cgi_out_fd(int cgi_out_fd, Client &client);
	void	add_cgi_in_fd(int cgi_in_fd, Client &client);
	void 	remove_fd(int fd);

	void	set_pollout(int fd);
	void	unset_pollout(int fd);

	std::list<Vserver> & get_vserver_lst(int listen_socket);

	fd_info & operator[](int fd);

	void debug_info() const;

};


#endif