#ifndef STATICRH_HPP
# define STATICRH_HPP

# include "includes.hpp"
# include "ARequestHandler.hpp"

class StaticRH: public ARequestHandler
{
private:
	int	fd_file;
	std::string header_str;
	HttpResponse response;

	void setup();
	void setup_404_response(); // TODO
	void setup_403_response(); // TODO
	void setup_200_response(struct stat &sb); // TODO
	std::string assemble_ressource_path() const;
	void assemble_header_str();
	int send_header();


public:
	StaticRH(HttpRequest *request, FdManager &table);
	~StaticRH();

	virtual int respond();
	virtual void abort();
};



#endif