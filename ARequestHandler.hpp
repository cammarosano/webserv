#ifndef AREQUESTHANDLER_HPP
# define AREQUESTHANDLER_HPP

# include "includes.hpp"
# include "FdManager.hpp"

enum e_rhstate
{
	s_setup, s_sending_header, s_start_send_file, s_sending_file, s_done
};

enum e_rhtype
{
	t_static, t_cgi
};

// construction takes a pointer to heap allocated HttpRequest, which is freed
// upon destruction
class ARequestHandler
{
protected:
	int			type; // static , cgi_get, cgi_post
	e_rhstate	state;
	HttpRequest	*request;
	FdManager	&table;

public:
	ARequestHandler(HttpRequest *request, FdManager &table);
	~ARequestHandler();

	virtual int respond() = 0;
	virtual void abort() = 0;
};

ARequestHandler::ARequestHandler(HttpRequest *request, FdManager &table):
request(request), table(table)
{
}

ARequestHandler::~ARequestHandler()
{
	delete request;
}


#endif