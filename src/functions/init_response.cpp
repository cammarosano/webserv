#include "includes.hpp"

// request.route cannot be NULL
// separates the URI into relative_part + query string
// assembles path removing the route prefix from the relative-part
std::string assemble_ressource_path(HttpRequest &request) {
    Route &r = *request.route;
    std::string url = request.target;

    url = url.substr(0, url.find('?')); // remove query string
    std::string path = r.root + '/' + url.substr(r.prefix.size());
    if (DEBUG)
        std::cout << "ressource path: " << path << std::endl;
    return (path);
}

// returns 1 if file, 2 if dir, 0 otherwise
int is_resource_available(std::string &path)
{
    struct stat sb;

    if (stat(path.c_str(), &sb) == -1)
        return (0);
    if (S_ISREG(sb.st_mode))
        return (1);
    if (S_ISDIR(sb.st_mode))
        return (2);
    return (0);
}

// initiates a response when the resource path is a directory
AReqHandler *directory_response(HttpRequest &request, FdManager &table,
                                std::string &resource_path) {
    Route &r = *request.route;

    // add slash to the end if necessary
    if (*resource_path.rbegin() != '/')
        resource_path += '/';
    // first, try default index file
    if (!r.default_index.empty()) // config file informs a default index
    {
        std::string default_index_path = resource_path + r.default_index;
        // check if file exists and is a regular file (no dir)
        if (is_resource_available(default_index_path) == 1) {
            // debug
            std::cout << "default index found: " << default_index_path
                      << std::endl;
            return (new StaticRH(&request, table, default_index_path));
        }
    }
    // if not found, do directory listing, if enabled
    if (r.auto_index)
        return (new DirectoryRH(&request, table, resource_path));
    // fall back to a 403
    return (new ErrorRH(&request, table, 403));
}

bool body_size_exceeds(HttpRequest &request) {
    std::map<std::string, std::string>::iterator it;
    size_t content_size;

    if (!request.route->body_size_limit) // 0 being considered no-limit
        return (false);
    it = request.header_fields.find("content-length");
    if (it == request.header_fields.end()) // not found, size unknown
        return (false);
    content_size = strtol(it->second.c_str(), NULL, 10);
    if (content_size >
        request.route->body_size_limit)
        return (true);
    return (false);
}

bool is_method_allowed(HttpRequest &request) {
    std::list<std::string>::iterator it; // obs: a set would be faster

    if (request.route->accepted_methods.empty() &&
        (request.method == "GET" || request.method == "HEAD"))
        return true;
    it = request.route->accepted_methods.begin();
    while (it != request.route->accepted_methods.end()) {
        if (*it == request.method)
            return (true);
        ++it;
    }
    return (false);
}


// check if path contains extension, removes aditional path and check
// if script exists
bool is_cgi_request(std::string resource_path, Route &route)
{
    // look for cgi extension and interpreter
    if (route.cgi_extension.empty())
        return (false);
    size_t pos = resource_path.find(route.cgi_extension);
    if (pos == std::string::npos) 
        return (false);

    // if anything after script name, must start with '/'
    size_t ext_len = route.cgi_extension.size();
    if (pos + ext_len > resource_path.size() && resource_path[pos + ext_len] != '/')
        return (false);

    // deduce script_path and check if available
    std::string script_path = resource_path.substr(0, pos + ext_len);
    if (is_resource_available(script_path) == 1) 
        return (true);
    return (false);
}

AReqHandler *cgi_response(HttpRequest &request, FdManager &table,
    std::string &resource_path)
{
    if (request.route->cgi_interpreter.empty())
        return (new ErrorRH(&request, table, 502)); // bad gateway
    if (request.method == "GET")
        return (new CgiGetRH(&request, table, resource_path));
    if (request.method == "POST")
        return (new CgiPostRH(&request, table, resource_path));
    return (new ErrorRH(&request, table, 501));
}

// resolve type of response: static_file, CGI, directory, error...
// instantiate the correct request handler
AReqHandler *init_response(HttpRequest &request, FdManager &table) {
    std::string resource_path;

    if (request.vserver->redirected)
        return (new RedirectRH(&request, table));
    if (!request.route)
        return (new ErrorRH(&request, table, 404));
    if (request.route->redirected)
        return (new RedirectRH(&request, table));
    if (!is_method_allowed(request))
        return (new ErrorRH(&request, table, 405));
    if (body_size_exceeds(request))
        return (new ErrorRH(&request, table, 413));
    resource_path = assemble_ressource_path(request);
    if (is_cgi_request(resource_path, *request.route))
        return (cgi_response(request, table, resource_path));
    // ressource not available not an error for post
    if (request.method == "POST") // upload
        return new PostRH(&request, table);
    // check if ressource is available
    int ret = is_resource_available(resource_path);
    if (ret == 0) // not available
        return (new ErrorRH(&request, table, 404));
    if (ret == 2) // directory
        return (directory_response(request, table, resource_path));
    // file
    if (request.method == "GET" || request.method == "HEAD")
        return (new StaticRH(&request, table, resource_path));
    if (request.method == "DELETE") {
        return new DeleteRH(&request, table, resource_path);
    }
    return (new ErrorRH(&request, table, 501));
}
