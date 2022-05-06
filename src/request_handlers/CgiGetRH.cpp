#include "CgiGetRH.hpp"

CgiGetRH::CgiGetRH(HttpRequest *request, FdManager &table,
				   std::string &script_path)
	: ACgiRH(request, table, script_path)
{
	if (setup() == -1)
		throw(std::exception());
	state = s_start;
}

CgiGetRH::~CgiGetRH()
{
	if (state > s_start && state < s_done)
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

	// make read-end non-blocking (fcntl)
	if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		close(pipefd[0]);
		close(pipefd[1]);
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

		// redirect stdout to the write-end of the pipe
		if (dup2(pipefd[1], 1) == -1)
		{
			perror("dup2");
			exit(1);
		}
		close(pipefd[0]);
		close(pipefd[1]);
		close(2);

		// chdir to cgi root
		if (chdir(request->route->root.c_str()) == -1)
			exit(1);
		execve(argv[0], argv, envp);
		perror("exec"); // if exec returns, that's an error
		exit(1);
	}

	close(pipefd[1]); // close write-end
	cgi_output_fd = pipefd[0];
	return (0);
}

int CgiGetRH::respond()
{
	switch (state)
	{
	case s_start:
		table.add_fd_read(cgi_output_fd, client);
		table.unset_pollout(client.socket); // make sure nothing's gonna be sent
		state = s_recv_cgi_header;

	case s_recv_cgi_header:
		if (client.unsent_data.empty())
		{
			if (table[cgi_output_fd].is_EOF) // CGI failed
				return (502);
			return (0);
		}
		response.status_code = 200;
		response.assemble_partial_header_str();
		// prepend header upper-part to content to be sent
		client.unsent_data.insert(0, response.header_str);
		state = s_recving_cgi_output;

	case s_recving_cgi_output:
		if (!table[cgi_output_fd].is_EOF) // not finished
			return (0);
		table.remove_fd(cgi_output_fd);
		state = s_done;

	default: // case s_done
		return (1);
	}
}

int CgiGetRH::time_out_code()
{
	return (504);
}
