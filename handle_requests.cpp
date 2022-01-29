#include "includes.hpp"

// this could be a method of a class HttpResponse
void assemble_header(HttpResponse &response)
{
	typedef std::map<std::string, std::string>::iterator iterator;

	// status-line
	response.header_str 	= response.http_version + ' '
							+ response.status_code_phrase + "\r\n";
	// header-fiels
	for (iterator it = response.header_fields.begin();
			it != response.header_fields.end(); ++it)
		response.header_str += it->first + ": " + it->second + "\r\n";

	// end header
	response.header_str += "\r\n";
}

// creates a HttpResponse object, and adds to the clients response queue 
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
	// TODO: handle the directory case
	if (ret == -1) // not found
	{
		response.status_code_phrase = "404 Not Found";
		response.source_type = none;
	}
	else
	{
		int fd = open(ressource_path.c_str(), O_RDONLY);
		if (fd == -1)
		{
			perror("open");
			response.status_code_phrase = "403 Forbidden";
			response.source_type = none;
		}
		else
		{
			response.status_code_phrase = "200 OK";
			response.fd_read = fd;
			response.source_type = file;
			response.header_fields["content-length"] = long_to_str(buf.st_size);

			// TODO: and many other header_fields here....
		}
	}
	assemble_header(response);
	response.state = sending_header;
	client.response_q.push(response);
	return (0);
}

int handle_requests(std::queue<HttpRequest> &q, std::map<int, Client> &clients)
{
	while (!q.empty())
	{
		HttpRequest &request = q.front();
		if (request.method == "GET")
		{
			handle_get_request(request, clients[request.client_socket]);
		}
		
		// else: unsupported method. for now, simply discard it

		q.pop();
	}
	return (0);
}