#include "StaticRH.hpp"

StaticRH::StaticRH(HttpRequest *request, FdManager &table):
ARequestHandler(request, table) 
{
	state = s_setup;
}

StaticRH::~StaticRH()
{
}

std::string StaticRH::assemble_ressource_path() const
{
	std::string route_root = request->route->root;
	std::string route_prefix = request->route->prefix;
	std::string path = route_root + '/' + 
						request->target.substr(route_prefix.length());
	// debug
	std::cout << "ressource path: " << path << std::endl;

	return (path);

}

// get fd for ressource, generate response header
void StaticRH::setup()
{
	std::string ressource_path;
	struct stat sb;

	// assemble ressource_path
	if (request->route == NULL)
	{
		setup_404_response();
		return;
	}
	ressource_path = assemble_ressource_path();

	// check if ressource is available
	if (stat(ressource_path.c_str(), &sb) == -1) // not found 
	{
		setup_404_response();
		return;
	}
	if (S_ISDIR(sb.st_mode)) // is a directory
	{
		setup_403_response(); // for now, no directory listing
		return;
	}
	fd_file = open(ressource_path.c_str(), O_RDONLY); // no access
	if (fd_file == -1)
	{
		setup_403_response();
		return;
	}

	setup_200_response(sb);
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

void StaticRH::setup_200_response(struct stat &sb)
{
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "200 OK";
	response.header_fields["content-length"] = long_to_str(sb.st_size);
	// TODO: and many other header_fields here.....
	assemble_header_str();
}

// TODO: check if vserver does not have a different default_403 page
void StaticRH::setup_403_response()
{
	struct stat sb;

	fd_file = open(DEFAULT_403_PAGE, O_RDONLY);
	if (fd_file == -1) // TODO: handle this
	{
		perror("open() default 403 page");
	}
	if (stat(DEFAULT_403_PAGE, &sb) == -1) // TODO: handle this
	{
		perror("stat() default 403 page");
	}
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "403 Forbidden";
	response.header_fields["content-length"] = long_to_str(sb.st_size);
	assemble_header_str();
}

// ! repeated code ! TODO: refactor this...
void StaticRH::setup_404_response()
{
	struct stat sb;

	fd_file = open(DEFAULT_404_PAGE, O_RDONLY);
	if (fd_file == -1) // TODO: handle this
	{
		perror("open() default 404 page");
	}
	if (stat(DEFAULT_404_PAGE, &sb) == -1) // TODO: handle this
	{
		perror("stat() default 404 page");
	}
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "404 Not Found";
	response.header_fields["content-length"] = long_to_str(sb.st_size);
	assemble_header_str();
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
// clear ressources and change state to s_done
void StaticRH::abort()
{
	table.remove_fd(fd_file);
	state = s_abort; // so it can be removed from the list of request_handlers
}
