#ifndef AREQUESTHANDLER_HPP
# define AREQUESTHANDLER_HPP

# include "FdManager.hpp"
# include "HttpRequest.hpp"
# include "macros.h"

/* 
Abstract class for request handlers
Request handlers for a specific kind of response (ex: serve a static file)
inherit from this class and define the respond() and abort() methods
*/
class ARequestHandler
{
protected:
	HttpRequest	*request;
	FdManager	&table;
	std::string header_str;

	enum e_rhstate
	{
		s_setup, s_sending_header, s_start_send_file, s_sending_file, s_done,
		s_abort, s_sending_html_str 
	} state;

	struct HttpResponse {
		std::string http_version;
		std::string status_code_phrase;
		std::map<std::string, std::string> header_fields;
	} response;

	void assemble_header_str();
	int send_header();

public:
	ARequestHandler(HttpRequest *request, FdManager &table);
	virtual ~ARequestHandler();

	virtual int respond() = 0;
	virtual void abort() = 0;

	HttpRequest * getRequest();
};

#endif