#include "includes.hpp"

Vserver & resolve_vserver(HttpRequest &request)
{
	typedef std::list<Vserver>::iterator iter_vserver_list;
	typedef std::list<std::string>::iterator iter_string_list;

	// this might create an empty string as "host" (no problem)
	std::string req_host_name = request.header_fields["host"];

	// remove eventual ":port_number" after the server name
	req_host_name = req_host_name.substr(0, req_host_name.find(':'));

	std::list<Vserver> &vservers_list = request.client.vservers;

	// iterate over list of vservers
	for (iter_vserver_list it = vservers_list.begin();
			it != vservers_list.end(); ++it)
	{
		Vserver &vserver = *it;

		// iterate over list of server_names
		for (iter_string_list it = vserver.server_names.begin();
			it != vserver.server_names.end(); ++it)
		{
			if (*it == req_host_name)
				return (vserver);
		}
	}
	// if there was no match, return the default vserver (first of list)
	return (vservers_list.front());
}

// location must end with a '/'
// look for the longest match between req_target and route prefixes
// returns NULL if no matching routes
Route * resolve_route(Vserver &vserver, std::string &request_target)
{
	std::list<Route> &routes = vserver.routes;

	Route *best_match = NULL;

	for (std::list<Route>::iterator it = routes.begin();
		 it != routes.end(); ++it)
	{
		std::string &route_prefix = it->prefix;
		size_t prefix_len = route_prefix.length();
		// if match
		if (request_target.substr(0, prefix_len) == route_prefix)
		{
			// if better match than before, update best_match
			if (!best_match || best_match->prefix.length() < prefix_len)
				best_match = &(*it);
		}
	}
	return (best_match);	
}
