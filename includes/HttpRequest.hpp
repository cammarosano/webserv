#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "includes.hpp"

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