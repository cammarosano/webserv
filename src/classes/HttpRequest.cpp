#include "HttpRequest.hpp"

HttpRequest::HttpRequest(Client &client, std::string &header_str):
client(client)
{
    parse_header(header_str);
    resolve_vserver();
    resolve_route();

    // log
    std::string host;
    std::map<std::string, std::string>::iterator it
        = header_fields.find("host");
    if (it != header_fields.end())
        host = it->second;
    else // no "host" header-field
        host = vserver->listen.first + ':'
            + long_to_str(vserver->listen.second);

    std::cout << "Request: " << method << " " << target << " " << http_version
        << " @ " << host << std::endl;
}

void HttpRequest::parse_header(std::string &header_str)
{
    std::istringstream stream(header_str);
    std::string line;

	// parse first_line
    getline(stream, line);
    std::istringstream first_line_stream(line);
    first_line_stream >> method;
    first_line_stream >> target;
    first_line_stream >> http_version;
    
    // parse header-fields
    while (!stream.eof())
    {
        getline(stream, line);
        size_t pos = line.find(':');
        if (pos == std::string::npos || pos == line.size() - 1) 
            continue; // invalid line, skipping
        std::string field_name = str_tolower(line.substr(0, pos));
        std::string field_value = line.substr(pos + 1);
        remove_trailing_spaces(field_value);
        header_fields[field_name] = field_value;
    }
}

void HttpRequest::resolve_vserver() {
    typedef std::list<Vserver>::iterator iter_vserver_list;
    typedef std::list<std::string>::iterator iter_string_list;
    typedef std::map<std::string, std::string>::iterator iter_map;
    std::list<Vserver> &vservers_list = client.vservers;

    iter_map it_host = header_fields.find("host");
    if (it_host == header_fields.end()) // no "host" header-field
    {
        vserver = &vservers_list.front();
        return ;    
    }
    std::string req_host_name = it_host->second;

    // remove eventual ":port_number" after the server name
    req_host_name = req_host_name.substr(0, req_host_name.find(':'));

    // iterate over list of vservers
    for (iter_vserver_list it = vservers_list.begin();
         it != vservers_list.end(); ++it) {
        Vserver &vs = *it;

        // iterate over list of server_names
        for (iter_string_list it = vs.server_names.begin();
             it != vs.server_names.end(); ++it) {
            if (*it == req_host_name) {
                vserver = &vs;
                return;
            }
        }
    }
    // if there was no match, pick default vserver (first of list)
    vserver = &vservers_list.front();
}

// location must end with a '/'
// look for the longest match between req_target and route prefixes
// NULL if no matching routes
void HttpRequest::resolve_route() {
    std::list<Route> &routes = vserver->routes;

    Route *best_match = NULL;

    for (std::list<Route>::iterator it = routes.begin(); it != routes.end();
         ++it) {
        std::string &route_prefix = it->prefix;
        size_t prefix_len = route_prefix.length();
        // if match
        if (target.substr(0, prefix_len) == route_prefix) {
            // if better match than before, update best_match
            if (!best_match || best_match->prefix.length() < prefix_len)
                best_match = &(*it);
        }
    }
    route = best_match;
}
