#include "CgiPostRH.hpp"

CgiPostRH::CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path):
ACgiRH(request, table, script_path),
bd(*request)
{
}

CgiPostRH::~CgiPostRH()
{
}

int CgiPostRH::setup()
{
	int pipe_in[2];
	int pipe_out[2];

	// open pipe for cgi input
	if (pipe(pipe_in) == -1)
	{
		perror("pipe");
		return (-1);
	}

	// open pipe for cgi output
	if (pipe(pipe_out) == -1)
	{
		perror("pipe");
		return (-1);
	}
	
	// fork
	pid_cgi_process = fork();
    if (pid_cgi_process == -1)
    {
        perror("fork");
        return (-1);
	}

	if (pid_cgi_process == 0) // child process
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

		// redirect stdin to the read-end of pipe_in
		if (dup2(pipe_in[0], 0) == -1)
        {
            perror("dup2");
            exit(1);
        }

		// redirect stdout to the write-end of pipe_out
		if (dup2(pipe_out[1], 1) == -1)
		{
            perror("dup2");
            exit(1);
		}
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);

        // chdir to cgi root ("correct directory" ??)
        chdir(request->route->root.c_str());

		// exec()
		execve(argv[0], argv, envp);
		
        // if exec returns, that's an error
        perror("exec");
        exit(1);
	}

	// parent process

	if (DEBUG)
		std::cout << "pid cgi process: " << pid_cgi_process << std::endl;
	close(pipe_out[1]); // close pipe out write end
	close(pipe_in[0]); // close pipe in read end
	cgi_output_fd = pipe_out[0];
	cgi_input_fd = pipe_in[1];
	table.add_cgi_out_fd(cgi_output_fd, request->client);
	table.add_cgi_in_fd(cgi_input_fd, request->client);
	return (0);
}

int CgiPostRH::respond()
{
	if (state == s_setup)
	{
		if (setup() == -1)
			return (-1);
		state = s_recv_req_body;
	}
	if (state == s_recv_req_body)
	{
		if (bd.decode_body() == 1) // TODO: handle errors (-1)
			state = s_sending_body2cgi;
		if (!request->client.decoded_body.empty())
			table.set_pollout(cgi_input_fd);
	}
	if (state == s_sending_body2cgi)
	{
		if (request->client.decoded_body.empty())
		{
			close(cgi_input_fd); // close write-end of input pipe to send EOF
			table.remove_fd(cgi_input_fd);
			state = s_recving_cgi_output;
		}
	}
	if (state == s_recving_cgi_output)
	{
		if (table[cgi_output_fd].is_EOF)
		{
			clear_resources();
			state = s_done;
		}
	}
	if (state == s_done)
		return (1);
    if (state == s_abort)
		return (-1);
	return (0);
}

void CgiPostRH::abort()
{
	if (state != s_recving_cgi_output)
	{
		close(cgi_input_fd);
		table.remove_fd(cgi_input_fd);
	}
	clear_resources();
	state = s_abort;
}
