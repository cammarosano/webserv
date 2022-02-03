#include "ARequestHandler.hpp"

ARequestHandler::ARequestHandler(HttpRequest *request, FdManager &table):
request(request), table(table)
{
}

ARequestHandler::~ARequestHandler()
{
	delete request;
}

Client & ARequestHandler::getClient()
{
	return request->client;
}
