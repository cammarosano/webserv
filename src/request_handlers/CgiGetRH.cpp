#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
                    std::string &script_path):
ACgiRH(request, table, script_path)
{
    if (setup() == -1)
        throw (std::exception());
    state = s_start;
}

CgiGetRH::~CgiGetRH()
{
    release_resources();
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
    cgi_process.pid = fork();
    if (cgi_process.pid == -1)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return (-1);
    }

    if (cgi_process.pid == 0) // child process
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
        
        close(2); // hide errors

        // exec()
        execve(argv[0], argv, envp);

        // if exec returns, that's an error
        perror("exec");
        exit(1);
    }

    // parent process
    if (DEBUG)
        std::cout << "pid cgi process: " << cgi_process.pid << std::endl;
    close(pipefd[1]); // close write-end
    cgi_output_fd = pipefd[0];
    return (0);
}

int CgiGetRH::respond()
{
    if (state == s_abort)
        return (-1);

    switch (state)
    {
    case s_start:
        table.add_fd_read(cgi_output_fd, request->client);
        state = s_recving_cgi_output;

    case s_recving_cgi_output:
        if (!table[cgi_output_fd].is_EOF) // not finished
            return (0);
        // if (cgi_failed() || bytes_sent == 0)
        if (bytes_sent == 0)
            return (502);
        table.remove_fd(cgi_output_fd);
        close(cgi_output_fd);
        state = s_done;

    default: // case s_done:
        return (1);
    }
}

void CgiGetRH::abort()
{
    release_resources();
    state = s_abort;
}

void CgiGetRH::release_resources()
{
    if (state < s_done)
    {
        table.remove_fd(cgi_output_fd);
        close(cgi_output_fd);
    }
    if (state < s_abort && !cgi_process.wait_done)
        table.add_child_to_reap(cgi_process);
}
