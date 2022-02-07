#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "includes.hpp"

/*
Holds all info from the HEADER of an HTTP request.
Upon construction, a string containing the header of a request is parsed,
the virtual server is resolved (based on the "host" header-field) and the route
is resolved (based on the "target" field).
*/
struct HttpRequest
{
	Client &client;
	Vserver *vserver; // resolved virutal server
	Route	*route; // resolved route
	std::string method, target, http_version; // request-line
	std::map<std::string, std::string> header_fields;

	HttpRequest(Client &client, std::string &header_str);

private:
	int	parse_header(std::string &header_str);
	void resolve_vserver();
	void resolve_route();
};

#endif