#ifndef ERRORRH_HPP
# define ERRORRH_HPP

# include "includes.hpp"
# include "ARequestHandler.hpp"

# define DEFAULT_403_PAGE "error_pages/403.html"
# define DEFAULT_404_PAGE "error_pages/404.html"

class ErrorRH: public ARequestHandler
{
private:
	int fd_file;

	int setup();

public:
	ErrorRH(HttpRequest *request, FdManager &table);
	~ErrorRH();

	virtual int respond();
	virtual void abort();

};

#endif