#include "HttpRequest.hpp"

#include "includes.hpp"

HttpRequest::HttpRequest(Client &client, std::string &header_str)
    : client(client) {
    parse_header(header_str);
    resolve_vserver();
    resolve_route();
    setup_cgi_env();

    // log
    std::cout << "Request: " << method << " " << target << " " << http_version
        << " @ " << header_fields["host"] << std::endl;
}

// TODO: this parser must be improved: check errors...
int HttpRequest::parse_header(std::string &header_str) {
    std::istringstream stream(header_str);

    stream >> method;
    stream >> target;
    stream >> http_version;
    while (!stream.eof()) {
        std::string line;
        getline(stream, line);
        size_t delimiter_pos = line.find(':');
        if (delimiter_pos == std::string::npos)  // invalid line, skipping
            continue;
        std::string field_name = str_tolower(line.substr(0, delimiter_pos));
        std::string field_value = line.substr(delimiter_pos + 1);
        remove_trailing_spaces(field_value);
        header_fields[field_name] = field_value;
    }
    return (0);
}

void HttpRequest::resolve_vserver() {
    typedef std::list<Vserver>::iterator iter_vserver_list;
    typedef std::list<std::string>::iterator iter_string_list;

    std::list<Vserver> &vservers_list = client.vservers;

    // this might create an empty string as "host" (no problem)
    // actually, maybe this is demanded by HTTP/1.1 ... TODO: check it!
    std::string req_host_name = header_fields["host"];
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

void HttpRequest::setup_cgi_env() {
    size_t c = header_fields["host"].find(":");
    std::string port = header_fields["host"].substr(c + 1, std::string::npos);
    std::string server_name = header_fields["host"].substr(0, c);

    cgi_env["SERVER_PORT"] = port;
    cgi_env["SERVER_NAME"] = server_name;
    cgi_env["SERVER_SOFTWARE"] = "webserv/1.1";
    cgi_env["SERVER_PROTOCOL"] = http_version;
    cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    cgi_env["REQUEST_METHOD"] = method;
    // cgi_env["SCRIPT_NAME"] = route->cgi_interpreter; // shouldn't it be the actual SCRIPT's name?
    // cgi_env["DOCUMENT_ROOT"] = route->root;
}