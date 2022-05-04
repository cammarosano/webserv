#ifndef AREQUESTHANDLER_HPP
#define AREQUESTHANDLER_HPP

#include "FdManager.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "macros.h"
#include "utils.h"

// forward declaration
struct HttpRequest;
class FdManager;

/*
Abstract class for request handlers
Request handlers for a specific kind of response (ex: serve a static file)
inherit from this class and define the respond() method
*/
class AReqHandler
{
  protected:
	HttpRequest *request;
	Client &client;
	FdManager &table;
	HttpResponse response;

	int send_str(std::string &str);
	bool response100_expected();

  public:
	AReqHandler(HttpRequest *request, FdManager &table);
	virtual ~AReqHandler();

	virtual int respond() = 0;
	virtual int time_out_code();

	bool keep_alive;
};

#endif
