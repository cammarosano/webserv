#include "ACgiRH.hpp"

ACgiRH::ACgiRH(HttpRequest *request, FdManager &table,
                std::string &script_path):
ARequestHandler(request, table),
script_path(script_path)
{
    query_str = get_query_str();
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

char **ACgiRH::setup_cgi_argv()
{
    char **argv = new char*[3];

    // argv[0] = CGI binary
    std::string& cgi_interp = request->route->cgi_interpreter;
    argv[0] = new char[cgi_interp.size() + 1];
    strcpy(argv[0], cgi_interp.c_str());

    // argv[1] = the script, having the the route's root/ removed
    std::string cgi_script
        = script_path.substr(request->route->root.size() + 1);
    argv[1] = new char[cgi_script.size() + 1];
    strcpy(argv[1], cgi_script.c_str());

    argv[2] = NULL;
    return (argv);
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

// https://datatracker.ietf.org/doc/html/rfc3875
std::map<std::string, std::string> ACgiRH::get_env_map()
{
    std::map<std::string, std::string> cgi_env;
    std::map<std::string, std::string>::iterator it;

    cgi_env["AUTH-TYPE"] = "";
    it = request->header_fields.find("content-length");
    if (it != request->header_fields.end())
        cgi_env["CONTENT_LENGTH"] = it->second;
    it = request->header_fields.find("content-type");
    if (it != request->header_fields.end())
        cgi_env["CONTENT_TYPE"] = it->second;
    cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    //  PATH_INFO: subject expects something different from the RFC3875
    cgi_env["PATH_INFO"] = script_path; 
    cgi_env["PATH-TRANSLATED"] = script_path; // no idea how to fill-in this one
    cgi_env["QUERY_STRING"] = query_str;
    cgi_env["REMOTE_ADDR"] = request->client.ipv4_addr;
    cgi_env["REMOTE_HOST"] = request->client.host_name;
    // OBS: skipping REMOTE_IDENT and REMOTE_USER
    cgi_env["REQUEST_METHOD"] = request->method;
    // SCRIPT_NAME = resource target without the query-string
    cgi_env["SCRIPT_NAME"] = 
        request->target.substr(0, request->target.find('?'));
    std::string host = request->header_fields["host"];
    cgi_env["SERVER_NAME"] = host.substr(0, host.find(':'));
    cgi_env["SERVER_PORT"] = long_to_str(request->vserver->listen.second);
    cgi_env["SERVER_PROTOCOL"] = request->http_version;
    cgi_env["SERVER_SOFTWARE"] = "webserv/1.1";

	// add HTTP meta variables
    it = request->header_fields.begin();
    while (it != request->header_fields.end())
    {
        if (!(it->first == "content-length" || it-> first == "content-type"))
            cgi_env["HTTP_" + meta_var_case(it->first)] = it->second;
        ++it;
    }
    return (cgi_env);
}

char **ACgiRH::setup_cgi_env()
{
    std::map<std::string, std::string> cgi_env = get_env_map();
    std::map<std::string, std::string>::iterator it;
    int i;

	// allocate memory - might throw exception!
    char **envp = new char *[cgi_env.size() + 1];

	// populate envp with content from cgi_env map
    it = cgi_env.begin();
    i = 0;
    while (it != cgi_env.end())
    {
        std::string s = it->first + '=' + it->second;
        envp[i] = new char[s.size() + 1];
        strcpy(envp[i], s.c_str());
        ++it;
        ++i;
    }
    envp[i] = NULL;
    return (envp);
}

// check if child process exited with something othen than 0
bool ACgiRH::is_cgi_error()
{
    int wstatus;
    int ret;

    ret = waitpid(pid_cgi_process, &wstatus, WNOHANG);
    if (ret == -1)
        perror("waitpid (is_cgi_error)");
    if (ret)
    {
        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
            return (true);
    }
    return (false);
}

// returns 1 if child process is finished and cleared
// returns -1 if child process is finished with error
// returns 0 otherwise, an sends SIGTERM to child
// does not block
int ACgiRH::wait_child()
{
    int wstatus;
    int ret;

    ret = waitpid(pid_cgi_process, &wstatus, WNOHANG);
    if (ret == 0) // has not exited
    {
        kill(pid_cgi_process, SIGTERM);
        return (0);
    }

    if (DEBUG) std::cout << "CGI process terminated" << std::endl;

    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus))
        return (-1);
    return (1);
}

void ACgiRH::send_502_response()
{
    std::string response(
            "HTTP/1.1 502 Bad Gateway\r\n"
            "Content-Length: 50\r\n"
            "\r\n"
            "<html><body><h1>502 Bad Gateway</h1></body></html>"
            "\r\n");
    request->client.unsent_data.append(response);
    table.set_pollout(request->client.socket);
    // log
    std::cout << "Response: 502 Bad Gateway" << std::endl;
}
