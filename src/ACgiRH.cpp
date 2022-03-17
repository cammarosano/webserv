#include "ACgiRH.hpp"

ACgiRH::ACgiRH(HttpRequest *request, FdManager &table,
                std::string &script_path):
ARequestHandler(request, table),
script_path(script_path)
{
    query_str = get_query_str();
    state = st_setup;
}

ACgiRH::~ACgiRH() {}

std::string ACgiRH::get_query_str()
{
    std::string query_string;

    size_t pos = request->target.find('?');
    if (pos != std::string::npos)
        query_string = request->target.substr(pos + 1);
    if (DEBUG)
        std::cout << "Query-string: " << query_string << std::endl;
    return (query_string);
}

void ACgiRH::setup_cgi_argv(char **argv)
{
    Route &route = *request->route;

    argv[0] = strdup(route.cgi_interpreter.c_str());
    // remove the root-prefix from script path, as the interpreter will
    // be run at that directory
    std::string arg1 = script_path.substr(route.root.size() + 1);
    argv[1] = strdup(arg1.c_str());
    argv[2] = NULL;
}

std::string meta_var_case(std::string s)
{
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (islower(s[i]))
            s[i] = toupper(s[i]);
        else if (s[i] == '-')
            s[i] = '_';
    }
    return (s);
}

// TODO: protect mallocs (strdup)
char **ACgiRH::setup_cgi_env()
{
    std::map<std::string, std::string> cgi_env;
    int i;

    cgi_env["SERVER_SOFTWARE"] = "webserv/1.1";
    std::string host = request->header_fields["host"];
    cgi_env["SERVER_NAME"] = host.substr(0, host.find(':'));
    cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    cgi_env["SERVER_PROTOCOL"] = request->http_version;
    cgi_env["SERVER_PORT"] = long_to_str(request->vserver->listen.second);
    cgi_env["REQUEST_METHOD"] = request->method;
    cgi_env["PATH_INFO"] = script_path;
    // SCRIPT_NAME = resource target without the query-string
    cgi_env["SCRIPT_NAME"] = request->target.substr(0, request->target.find('?'));
    if (!query_str.empty())
        cgi_env["QUERY_STRING"] = query_str;
    // cgi_env["DOCUMENT_ROOT"] = route->root;

    // TODO: other variables...

    std::map<std::string, std::string>::iterator it;
    it = request->header_fields.begin();
    while (it != request->header_fields.end())
    {
        cgi_env["HTTP_" + meta_var_case(it->first)] = it->second;
        ++it;
    }

	// allocate memory
    char **envp = new char *[cgi_env.size() + 1];

	// populate envp with content from cgi_env map
    it = cgi_env.begin();
    i = 0;
    while (it != cgi_env.end())
    {
        envp[i] = strdup((it->first + '=' + it->second).c_str());
        ++it;
        ++i;
    }
    envp[i] = NULL;
    return (envp);
}

void ACgiRH::clear_resources()
{
    // consider using SIGKILL to ensure termination
    kill(pid_cgi_process, SIGTERM);
    table.remove_fd(cgi_output_fd);
    close(cgi_output_fd); // close pipe's read-end
    // this line might block the program!
    waitpid(pid_cgi_process, NULL, 0);
    if (DEBUG) std::cout << "CGI process terminated" << std::endl;
}
