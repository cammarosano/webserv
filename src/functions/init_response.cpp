#include "includes.hpp"

// request.route cannot be NULL
// separates the URI into relative_part + query string
// assembles path removing the route prefix from the relative-part
std::string assemble_ressource_path(HttpRequest &request)
{
    Route &r = *request.route;
    std::string url = request.target;

    url = url.substr(0, url.find('?')); // remove query string
    std::string path = r.root + '/' + url.substr(r.prefix.size());
    if (DEBUG)
        std::cout << "ressource path: " << path << std::endl;
    return (path);
}

// initiates a response when the resource path is a directory
AReqHandler *directory_response(HttpRequest &request, FdManager &table, std::string &resource_path)
{
    struct stat sb;
    Route &r = *request.route;

    // add slash to the end if necessary
    if (*resource_path.rbegin() != '/')
        resource_path += '/';

    // first, try default index file
    if (!r.default_index.empty()) // config file informs a default index
    {
        std::string default_index_path = resource_path + r.default_index;
        // check if file exists and is a regular file (no dir)
        if (stat(default_index_path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
        {
            // debug
            std::cout << "default index found: " << default_index_path << std::endl;
            return (new StaticRH(&request, table, default_index_path));
        }
    }
    // if not found, do directory listing, if enabled
    if (r.auto_index)
        return (new DirectoryRH(&request, table, resource_path));
    // fall back to a 403
    return (new ErrorRH(&request, table, 403));
}

bool body_size_exceeds(HttpRequest &request)
{
    std::map<std::string, std::string>::iterator it;
    size_t content_size;

    if (!request.route->body_size_limit) // 0 being considered no-limit
        return (false);
    it = request.header_fields.find("content-length");
    if (it == request.header_fields.end()) // not found, size unknown
        return (false);
    content_size = strtol(it->second.c_str(), NULL, 10);
    if (content_size > request.route->body_size_limit * 1024 * 1024) // Megabytes
        return (true);
    return (false);
}

bool is_method_allowed(HttpRequest &request)
{
    std::list<std::string>::iterator it; // obs: a set would be faster

    it = request.route->accepted_methods.begin();
    if (request.route->accepted_methods.empty() && (request.method == "GET" || request.method == "HEAD"))
        return true;
    while (it != request.route->accepted_methods.end())
    {
        if (*it == request.method)
            return (true);
        ++it;
    }
    return (false);
}

// resolve type of response: static_file, CGI, directory, error...
// instantiate the correct request handler
AReqHandler *init_response(HttpRequest &request, FdManager &table)
{
    std::string resource_path;
    struct stat sb;

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
    // check if ressource is available
    if (stat(resource_path.c_str(), &sb) == -1) // resource not found
        return (new ErrorRH(&request, table, 404));
    if (S_ISDIR(sb.st_mode)) // check if it's a directory
        return (directory_response(request, table, resource_path));
    // check if regular file and has read rights (obs: access() is a security
    // hole!!)
    if (!S_ISREG(sb.st_mode) || access(resource_path.c_str(), R_OK))
        return (new ErrorRH(&request, table, 404));
    // check if CGI script (match extension)
    if (!request.route->cgi_extension.empty() && resource_path.find(request.route->cgi_extension) != std::string::npos)
    {
        if (request.method == "GET")
            return (new CgiGetRH(&request, table, resource_path));
        if (request.method == "POST")
            return (new CgiPostRH(&request, table, resource_path));
    }
    if (request.method == "POST")
        return new PostRH(&request, table);
    else if (request.method == "GET" || request.method == "HEAD")
        return (new StaticRH(&request, table, resource_path));
    else if (request.method == "DELETE")
    {
        return new DeleteRH(&request, table, resource_path);
    }
    return (new ErrorRH(&request, table, 501));
}
