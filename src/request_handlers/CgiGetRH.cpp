#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
                    std::string &script_path):
ACgiRH(request, table, script_path)
{
    if (setup() == -1)
        throw (std::exception());
    state = s_recving_cgi_output;
}

CgiGetRH::~CgiGetRH()
{
    table.remove_fd(cgi_output_fd);
    close(cgi_output_fd);
    if (waitpid(cgi_process, NULL, WNOHANG) == 0)
        child_processes.push_back(cgi_process);
}

int CgiGetRH::setup()
{
    int pipefd[2];

    // open pipe for cgi output
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return (-1);
    }

    // fork
    cgi_process = fork();
    if (cgi_process == -1)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return (-1);
    }

    if (cgi_process == 0) // child process
    {
        // setup argv and envp for execve
        char **argv = setup_cgi_argv();
        char **envp = setup_cgi_env();

        // debug
        if (DEBUG)
        {
            std::cout << "cgi interpreter path: " << argv[0] << std::endl;
            std::cout << "script path, relative to route's root: " << argv[1] << std::endl;
        }

        // redirect stdout to the write-end of the pipe
        if (dup2(pipefd[1], 1) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);

        // chdir to cgi root ("correct directory" ??)
        if (chdir(request->route->root.c_str()) == -1)
            exit(1);
        
        // close(2); // hide errors

        // exec()
        execve(argv[0], argv, envp);

        // if exec returns, that's an error
        perror("exec");
        exit(1);
    }

    // parent process
    if (DEBUG)
        std::cout << "pid cgi process: " << cgi_process << std::endl;
    close(pipefd[1]); // close write-end
    cgi_output_fd = pipefd[0];
    table.add_fd_read(cgi_output_fd, client);
    return (0);
}

int CgiGetRH::respond()
{
    if (!table[cgi_output_fd].is_EOF) // not finished
        return (0);
    if (bytes_recvd == 0) // GCI failed
        return (502);
    state = s_done;
    return (1);
}

int CgiGetRH::time_out_code()
{
    return (504);
}
