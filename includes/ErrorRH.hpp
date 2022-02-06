#ifndef ERRORRH_HPP
# define ERRORRH_HPP

# include "includes.hpp"
# include "ARequestHandler.hpp"

# define DEFAULT_403_PAGE "error_pages/403.html"
# define DEFAULT_404_PAGE "error_pages/404.html"

class ErrorRH: public ARequestHandler
{
private:
	int error_code;
	std::string html_page;

	int setup();
	void generate_error_page();
	int send_html_str();

	static std::map<int, std::string> reason_phrases;

public:
	ErrorRH(HttpRequest *request, FdManager &table, int error_code);
	~ErrorRH();

	virtual int respond();
	virtual void abort();

	static std::map<int, std::string> init_map();

};

#endif