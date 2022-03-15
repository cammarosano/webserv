#include "ACgiRH.hpp"

ACgiRH::ACgiRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query):
ARequestHandler(request, table),
script_path(script_path),
query(query)
{
    state = st_setup;
}

ACgiRH::~ACgiRH() {}

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

// TODO: protect mallocs (strdup)
void ACgiRH::setup_cgi_env(char **envp)
{
    std::map<std::string, std::string>::iterator it;
    int i;

    it = request->cgi_env.begin();
    i = 0;
    while (it != request->cgi_env.end())
    {
        envp[i] = strdup((it->first + '=' + it->second).c_str());
        ++it;
        ++i;
    }
    envp[i++] = strdup(("QUERY_STRING=" + query).c_str());
    envp[i] = NULL;
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
