#include "includes.hpp"

int handle_get_request(HttpRequest &request, Client &client)
{
	// TODO: resolve virtual server, resolve route
	std::string root = "web_root";

	// instantiate response
	HttpResponse response;
	response.http_version = "HTTP/1.1";

	std::string ressource_path = root + request.request_target;

	// check if ressource is available
	struct stat buf;
	int ret = stat(ressource_path.c_str(), &buf);
	if (ret == -1) // not found
		response.status_code_phrase = "404 Not Found";
	else
	{
		int fd = open(ressource_path.c_str(), O_RDONLY);
		if (fd == -1)
		{
			perror("open");
			response.status_code_phrase = "403 Forbidden";
		}
		else
		{
			response.status_code_phrase = "200 OK";
			response.fd_ressource = fd;
			response.bytes_left = buf.st_size;
			response.header_fields["content-length"] = long_to_str(response.bytes_left);
		}
	}

	client.response_queue.push(response);
	return (0);
}
