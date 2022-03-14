#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
                    std::string &script_path, std::string &query):
ACgiRH(request, table, script_path, query)
{
}

CgiGetRH::~CgiGetRH()
{
}

int CgiGetRH::setup()
{
    int pipefd[2];

    // open pipe for cgi output
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return -1;
    }

    // fork
    pid_cgi_process = fork();
    if (pid_cgi_process == -1)
    {
        perror("fork");
        return -1;
    }

    if (pid_cgi_process == 0) // child process
    {
        // setup argv and envp for execve
        char *argv[3];
        char *envp[20];
        setup_cgi_argv(argv);
        setup_cgi_env(envp);

        // debug
        std::cout << "cgi interpreter path: " << argv[0] << std::endl;
        std::cout << "script path, relative to route's root: " << argv[1] << std::endl;

        // redirect stdout to the write-end of the pipe
        if (dup2(pipefd[1], 1) == -1)
        {
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
    close(pipefd[1]); // close write-end
    cgi_output_fd = pipefd[0];
    table.add_cgi_out_fd(cgi_output_fd, request->client);
    return (0);
}

int CgiGetRH::respond()
{
    if (state == st_setup)
    {
        if (setup() == -1)
            return -1;
        state = st_recving_cgi_output;
    }
    if (state == st_recving_cgi_output)
    {
        if (table[cgi_output_fd].is_EOF)
        {
            clear_resources();
            state = st_done;
        }
    }
    if (state == st_done)
        return (1);
    if (state == st_abort)
        return (-1);
    return (0);
}

void CgiGetRH::abort()
{
    clear_resources();
    state = st_abort;
}




