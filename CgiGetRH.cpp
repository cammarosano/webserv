#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
                   std::string &script_path, std::string &query)
    : ARequestHandler(request, table), script_path(script_path), query(query) {
    state = s_setup;
}

CgiGetRH::~CgiGetRH() {}

void CgiGetRH::setup_cgi_argv(char **argv) {
    Route &route = *request->route;

    argv[0] = strdup(route.cgi_interpreter.c_str());
    // remove the root-prefix from script path, as the interpreter will
    // be run at that directory
    std::string arg1 = script_path.substr(route.root.size() + 1);
    argv[1] = strdup(arg1.c_str());
    argv[2] = NULL;
}

// TODO: protect mallocs (strdup)
void CgiGetRH::setup_cgi_env(char **envp) {
    std::map<std::string, std::string>::iterator it;
    int i;

    it = request->cgi_env.begin();
    i = 0;
    while (it != request->cgi_env.end()) {
        envp[i] = strdup((it->first + '=' + it->second).c_str());
        ++it;
        ++i;
    }
    envp[i++] = strdup(("QUERY_STRING=" + query).c_str());
    envp[i] = NULL;
}

int CgiGetRH::setup() {
    int pipefd[2];

    // open pipe for cgi output
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }
    // std::cout << "pipe fds: " << pipefd[0] << " and " << pipefd[1] <<
    // std::endl;

    // fork
    pid_cgi_process = fork();
    if (pid_cgi_process == -1) {
        perror("fork");
        return -1;
    }

    if (pid_cgi_process == 0)  // child process
    {
        // setup argv and envp for execve
        char *argv[3];
        char *envp[20];
        setup_cgi_argv(argv);
        setup_cgi_env(envp);

        // debug
        std::cout << "cgi interpreter path: " << argv[0] << std::endl;
        std::cout << "script path, relative to route's root: " << argv[1]
                  << std::endl;

        // redirect stdout to the write-end of the pipe
        if (dup2(pipefd[1], 1) == -1) {
            perror("dup2");
            exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);

        // chdir to cgi root ("correct directory" ??)
        chdir(request->route->root.c_str());

        // exec()
        execve(argv[0], argv, envp);

        // if exec returns, that's an error
        perror("exec");
        exit(1);
    }

    // parent process
    std::cout << "pid cgi process: " << pid_cgi_process << std::endl;
    close(pipefd[1]);  // close write-end
    cgi_output_fd = pipefd[0];
    return (0);
}

int CgiGetRH::respond() {
    if (state == s_setup) {
        if (setup() == -1) return -1;
        table.add_cgi_out_fd(cgi_output_fd, request->client);
        state = s_sending_cgi_output;
    }
    if (state == s_sending_cgi_output) {
        if (table[cgi_output_fd].is_EOF) {
            kill(pid_cgi_process,
                 SIGTERM);  // consider using SIGKILL to ensure termination
            table.remove_fd(cgi_output_fd);
            close(cgi_output_fd);  // close pipe's read-end
            // this line might block the program!
            waitpid(pid_cgi_process, NULL, 0);
            std::cout << "CGI process terminated" << std::endl;
            state = s_done;
        }
    }
    // I think this might block the program at poll...
    // as there's no fd change expected to signal this. If poll times out, then
    // ok, maybe waiting from child processes to terminate should be done
    // outside of RHs. if (state == s_waiting_child)
    // {
    // 	int wstatus;

    // 	// check if child process is terminated
    // 	int ret = waitpid(pid_cgi_process, &wstatus, WNOHANG);

    // 	if (ret == -1)
    // 	{
    // 		perror("waitpid");
    // 		return (-1);
    // 	}
    // 	if (ret > 0) // process exited
    // 	{
    // 		state = s_done;
    // 	}
    // }
    if (state == s_done) return (1);
    if (state == s_abort) return (-1);
    return (0);
}

void CgiGetRH::abort() {
    // TODO
}
