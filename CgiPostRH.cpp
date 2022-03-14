#include "CgiPostRH.hpp"

CgiPostRH::CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query):
ACgiRH(request, table, script_path, query)
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
		char *argv[3];
		char *envp[20];
		setup_cgi_argv(argv);
		setup_cgi_env(envp);

        // debug
        std::cout << "cgi interpreter path: " << argv[0] << std::endl;
        std::cout << "script path, relative to route's root: " << argv[1] << std::endl;

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
    std::cout << "pid cgi process: " << pid_cgi_process << std::endl;
	close(pipe_out[1]); // close pipe out write end
	close(pipe_in[0]); // close pipe in read end
	cgi_output_fd = pipe_out[0];
	cgi_input_fd = pipe_in[1];
	table.add_cgi_out_fd(cgi_output_fd, request->client);
	table.add_cgi_in_fd(cgi_input_fd, request->client);
	return (0);
}

// assuming "content-length" header-field exists
int CgiPostRH::get_body_len()
{
	const char *cont_len = request->header_fields["content-length"].c_str();
	return (strtol(cont_len, NULL, 10));
}

// transfer request body content from Client's received_data to req_body_data.
// return 1 if complete, 0 if incomplete.
int CgiPostRH::send_body2cgi()
{
	std::string &received_data = request->client.received_data;
	std::string &req_body_data = request->client.req_body_data;
	int max_bytes = BUFFER_SIZE - req_body_data.size();
	int bytes_available = received_data.size();
	int n_bytes;

	// calculate amount of bytes to be transfered
	n_bytes = body_len_left;
	if (bytes_available < n_bytes)
		n_bytes = bytes_available;
	if (n_bytes > max_bytes)
		n_bytes = max_bytes;
	
	if (n_bytes <= 0)
		return (0);

	// transfer
	req_body_data.append(received_data, 0, n_bytes);
	received_data.erase(0, n_bytes);
	// set POLLOUT
	table.set_pollout(cgi_input_fd);
	// update body length left
	body_len_left -= n_bytes;

	if (!body_len_left)
		return (1);
	return (0);
}

int CgiPostRH::respond()
{
	if (state == st_setup)
	{
		if (setup() == -1)
			return (-1);

		// TODO: resolve body type: known len or chuncked
		// for now, know_len only
		body_len_left = get_body_len();
		// debug
		std::cout << "parsed body-len: " << body_len_left << std::endl;
		state = st_get_req_body;
	}
	if (state == st_get_req_body)
	{
		if (send_body2cgi() == 1)
			state = st_sending_body2cgi;
	}
	if (state == st_sending_body2cgi)
	{
		if (request->client.req_body_data.empty())
		{
			close(cgi_input_fd); // close write-end of input pipe to send EOF
			table.remove_fd(cgi_input_fd);
			state = st_recving_cgi_output;
		}
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

void CgiPostRH::abort()
{
	if (state != st_recving_cgi_output)
	{
		close(cgi_input_fd);
		table.remove_fd(cgi_input_fd);
	}
	clear_resources();
	state = st_abort;
}
