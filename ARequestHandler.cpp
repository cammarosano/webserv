#include "ARequestHandler.hpp"

ARequestHandler::ARequestHandler(HttpRequest *request, FdManager &table):
request(request), table(table)
{
}

ARequestHandler::~ARequestHandler()
{
}

HttpRequest * ARequestHandler::getRequest()
{
	return request;
}
