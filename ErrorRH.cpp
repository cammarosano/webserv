#include "ErrorRH.hpp"

ErrorRH::ErrorRH(HttpRequest *request, FdManager &table):
ARequestHandler(request, table)
{
	state = s_setup;
}

ErrorRH::~ErrorRH()
{
}

// TODO: default page, different errors codes...
int ErrorRH::setup()
{
	fd_file = open(DEFAULT_404_PAGE, O_RDONLY);
	if (fd_file == -1) 
	{
		// TODO, handle this
		return -1;
	}

	struct stat sb;

	fstat(fd_file, &sb); // todo: handle error
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "404 Not Found";
	response.header_fields["content-length"] = long_to_str(sb.st_size);
	// TODO: and many other header_fields here.....
	assemble_header_str();

	return (0);
}

// returns 1 if response if complete
// 0 if response not yet complete
// -1 if response was aborted
int ErrorRH::respond()
{
	if (state == s_setup)
	{
		setup();
		// TODO: handle errors
		state = s_sending_header;
	}
	if (state == s_sending_header)
	{
		if (send_header() == 1)
			state = s_start_send_file;
	}
	if (state == s_start_send_file)
	{
		table.add_file_fd(fd_file, request->client);
		state = s_sending_file;
		return 0;
	}
	if (state == s_sending_file)
	{
		if (table[fd_file].is_EOF)
		{
			close(fd_file);
			table.remove_fd(fd_file);
			state = s_done;
		}
	}
	if (state == s_done)
		return (1);
	if (state == s_abort)
		return (-1);
	return (0);
}

void ErrorRH::abort()
{
	close(fd_file);
	table.remove_fd(fd_file);
	state = s_abort; // so it can be removed from the list of request_handlers
}