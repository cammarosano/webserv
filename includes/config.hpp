#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <list>
#include <map>
#include <string>

struct Redirection
{
	std::string location;
	int status_code;
};

// equivalent to nginx's "location"
struct Route
{
	std::string prefix; // location
	std::list<std::string> accepted_methods;
	// todo: redirection
	std::string root;
	bool auto_index; // directory listing
	std::string default_index;

	bool redirected;
	Redirection redirect;

	// cgi
	std::string cgi_extension;
	std::string cgi_interpreter; // program name/path

	// upload
	bool upload_accepted;
	std::string upload_dir; // where to store
	size_t body_size_limit;

	// route error pages
	std::map<int, std::string> error_pages;

	Route(std::string prefix)
		: prefix(prefix), auto_index(false), redirected(false),
		  body_size_limit(0)
	{
	}
};

typedef std::pair<std::string, unsigned short> ip_port;

// equivalent to nginx's "server"
struct Vserver
{
	ip_port listen;
	bool redirected;
	std::list<std::string> server_names;
	std::string default_404; // default error page
	std::string default_403; // default error page
	Redirection redirect;

	// key is the code value is the page path ex 404 => 404.html
	std::map<int, std::string> err_pages;
	// etc...
	size_t body_size_limit;
	std::list<Route> routes;

	Vserver() : redirected(false)
	{
	}
};

#endif