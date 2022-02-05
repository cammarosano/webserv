#ifndef STATICRH_HPP
# define STATICRH_HPP

# include "includes.hpp"
# include "ARequestHandler.hpp"

# define DEFAULT_403_PAGE "error_pages/403.html"
# define DEFAULT_404_PAGE "error_pages/404.html"

class StaticRH: public ARequestHandler
{
private:
	int	fd_file;
	std::string header_str;
	HttpResponse response;
	std::string resource_path;

	int setup();

protected:
	void assemble_header_str();
	int send_header();

public:
	StaticRH(HttpRequest *request, FdManager &table,
				std::string &resource_path);
	~StaticRH();

	virtual int respond();
	virtual void abort();
};



#endif