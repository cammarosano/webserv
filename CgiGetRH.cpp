#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query):
ARequestHandler(request, table), script_path(script_path), query(query)
{
	state = s_setup;
}

CgiGetRH::~CgiGetRH()
{
}

// use cgi+_env from the HttpRequest
void CgiGetRH::setup_cgi_env(char **envp)
{
	envp[0] = strdup(("REQUEST_METHOD=" + request->method).c_str());
	envp[1] = strdup(("QUERY_STRING=" + query).c_str());

	envp[2] = NULL;
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
	std::cout << "pipe fds: " << pipefd[0] << " and " << pipefd[1] << std::endl;

	// fork 
	pid_cgi_process = fork();
	if (pid_cgi_process == -1)
	{
		perror("fork");
		return -1;
	}

	if (pid_cgi_process == 0) // child process
	{
		std::cerr << "about to exec" << std::endl;
		std::cerr << "cgi interpreter path: " << request->route->cgi_interpreter << std::endl;
		std::cerr << "script path: " << script_path << std::endl;

		close(pipefd[0]); // close read-end

		// redirect stdout to the write-end of the pipe
		if (dup2(pipefd[1],1) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(pipefd[1]);

		// setup argv for cgi
		char *argv[3];
		argv[0] = strdup(request->route->cgi_interpreter.c_str());
		argv[1] = strdup(script_path.c_str());
		argv[2] = NULL;

		// setup env for cgi
		char *envp[20];
		setup_cgi_env(envp);

		// TODO: chdir to cgi root
		// chdir(request->route->root.c_str());

		// exec()
		execve(request->route->cgi_interpreter.c_str(), argv, envp);

		// if exec returns, that's an error
		perror("exec");
		exit(1);
	}
	
	// parent process
	std::cout << "pid cgi process: " << pid_cgi_process << std::endl;
	close(pipefd[1]); // close write-end
	cgi_output_fd = pipefd[0];
	return (0);

}

int CgiGetRH::respond()
{
	if (state == s_setup)
	{
		if (setup() == -1)
			return -1;
		table.add_cgi_out_fd(cgi_output_fd, request->client);
		state = s_sending_cgi_output;
	}
	if (state == s_sending_cgi_output)
	{
		int wstatus;

		// check if child process is terminated 
		// OBS: this seems to be slow, it takes many cycles to get a positive return
		// by waitpid. Consider checking for EOF and remove fd from table!
		int ret = waitpid(pid_cgi_process, &wstatus, WNOHANG);

		if (ret == -1)
		{
			perror("waitpid");
			return (-1);
		}
		if (ret > 0) // process exited
		{
			std::cout << "CGI process terminated" << std::endl;
			close(cgi_output_fd); // close pipe's read-end
			table.remove_fd(cgi_output_fd);
			state = s_done;
		}
	}
    if (state == s_done)
		return (1);
	return (0);
}

void CgiGetRH::abort()
{
	// TODO
}
