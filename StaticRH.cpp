#include "StaticRH.hpp"

// TODO: receive ressource_path as a parameter
StaticRH::StaticRH(HttpRequest *request, FdManager &table,
	std::string &resource_path):
ARequestHandler(request, table), resource_path(resource_path)
{
	state = s_setup;
}

StaticRH::~StaticRH()
{
}

// get fd for ressource, generate response header
// TODO: FIX THIS MESS
int StaticRH::setup()
{
	fd_file = open(resource_path.c_str(), O_RDONLY); // no access
	if (fd_file == -1)
	{
		// TODO, handle this
		return -1;
	}

	struct stat sb;

	stat(resource_path.c_str(), &sb); // todo: handle error
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "200 OK";
	response.header_fields["content-length"] = long_to_str(sb.st_size);
	// TODO: and many other header_fields here.....
	assemble_header_str();

	return (0); // ok
}

void StaticRH::assemble_header_str()
{
	typedef std::map<std::string, std::string>::iterator iterator;

	// status-line
	header_str 	= response.http_version + ' '
							+ response.status_code_phrase + "\r\n";
	// header-fiels
	for (iterator it = response.header_fields.begin();
			it != response.header_fields.end(); ++it)
		header_str += it->first + ": " + it->second + "\r\n";

	// end header
	header_str += "\r\n";
}



// transfer header_str to Client's unsent_data buffer
// return 1 if complete, 0 if incomplete
int StaticRH::send_header()
{
	Client &client = request->client;
	int max_bytes;
	
	max_bytes = BUFFER_SIZE - client.unsent_data.size();
	if (max_bytes <= 0) // buffer is full
		return 0;
	client.unsent_data += header_str.substr(0, max_bytes);
	header_str.erase(0, max_bytes);
	table.set_pollout(client.socket);
	if (header_str.empty())
		return 1;
	return 0;
}

// perform the necessary sequence of steps to respond to a request
// returns 1 if response if complete
// 0 if response not yet complete
// -1 if response was aborted
int StaticRH::respond()
{
	if (state == s_setup) // this could be moved to the constructor
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

// to be used in case of abrubt connection close from the client side
// clear ressources and change state
void StaticRH::abort()
{
	close(fd_file);
	table.remove_fd(fd_file);
	state = s_abort; // so it can be removed from the list of request_handlers
}
