#include "CgiPostRH.hpp"

CgiPostRH::CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path):
ACgiRH(request, table, script_path), bd(*request)
{
	if (setup() == -1)
		throw (std::exception());
	state = s_start;
}

CgiPostRH::~CgiPostRH()
{
	release_resources();
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
		close(pipe_in[0]);
		close(pipe_in[1]);
		return (-1);
	}
	
	// fork
	cgi_process.pid = fork();
    if (cgi_process.pid == -1)
    {
        perror("fork");
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
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
		std::cout << "pid cgi process: " << cgi_process.pid << std::endl;
	close(pipe_out[1]); // close pipe out write end
	close(pipe_in[0]); // close pipe in read end
	cgi_output_fd = pipe_out[0];
	cgi_input_fd = pipe_in[1];
	return (0);
}

int CgiPostRH::respond()
{
	int ret_bd;

	if (state == s_abort)
		return (-1);
	if (!cgi_process.wait_done && cgi_failed())
		return (502); // release_ressources done by destructor

	switch (state)
	{
	case s_start:
		table.add_cgi_out_fd(cgi_output_fd, request->client);
		table.add_cgi_in_fd(cgi_input_fd, request->client);
		state = s_recv_req_body;

	case s_recv_req_body:
		ret_bd = bd.decode_body();
		if (ret_bd == -1) // error
			return (400);
		if (!client.decoded_body.empty()) // there's data to be sent to CGI
			table.set_pollout(cgi_input_fd);
		if (ret_bd == 0) // not finished
			return (0);
		state = s_sending_body2cgi;

	case s_sending_body2cgi:
		if (!client.decoded_body.empty()) // not finished
			return (0);
		close(cgi_input_fd); // sends EOF
		table.remove_fd(cgi_input_fd);
		state = s_recving_cgi_output;

	case s_recving_cgi_output:
		if (!table[cgi_output_fd].is_EOF) // not finished
			return (0);
		if (bytes_sent == 0)
			return (502);
		table.remove_fd(cgi_output_fd);
        close(cgi_output_fd);
		state = s_done;

	default: // case s_done
		return (1);
	}
}

void CgiPostRH::abort()
{
	release_resources();
	state = s_abort;
}

void CgiPostRH::release_resources()
{
	if (state < s_recving_cgi_output)
	{
		close(cgi_input_fd);
		table.remove_fd(cgi_input_fd);
	}
	if (state < s_done)
	{
		table.remove_fd(cgi_output_fd);
		close(cgi_output_fd);
	}
	if (state < s_abort && !cgi_process.wait_done)
		table.add_child_to_reap(cgi_process);
}
