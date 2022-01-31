#include "includes.hpp"

#define DEBUG_LOG std::cout << \
	"The following response header will be sent to client at " \
	<< "socket "  << request.client.socket << "\n" << \
	response.header_str << std::flush;

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

int issue_404_response(HttpRequest &request)
{
	// instantiate response
	HttpResponse response;

	// populate attributes
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "404 Not Found";
	response.source_type = none; // TODO: default error page (html)
	response.header_fields["content-length"] = "0";

	// make header_str
	assemble_header(response);

	// enqueue
	request.client.response_q.push(response);
	
	DEBUG_LOG
	return (0);
}

int issue_403_response(HttpRequest &request)
{
	// instantiate response
	HttpResponse response;

	// populate attributes
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "403 Forbidden";
	response.source_type = none;
	response.header_fields["content-length"] = "0";

	// make header_str
	assemble_header(response);

	// enqueue
	request.client.response_q.push(response);

	DEBUG_LOG
	return (0);
}

int issue_200_response(HttpRequest &request, int fd_file, struct stat &sb)
{
	// instantiate response
	HttpResponse response;

	// populate attributes
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "200 OK";
	response.source_type = file;
	response.fd_read = fd_file;
	response.header_fields["content-length"] = long_to_str(sb.st_size);
		// TODO: and many other header_fields here....

	// make header_str
	assemble_header(response);

	// enqueue
	request.client.response_q.push(response);

	DEBUG_LOG
	return (0);
}

// experimental!
int issue_200_post_resp(HttpRequest &request, int payload_size)
{
	HttpResponse response;

	// populate attributes
	response.http_version = "HTTP/1.1";
	response.status_code_phrase = "200 OK";
	response.source_type = qstring;
	response.header_fields["content-length"] = long_to_str(payload_size);
	response.header_fields["content-type"] = "text/plain";
		// TODO: and many other header_fields here....

	// make header_str
	assemble_header(response);

	// enqueue
	request.client.response_q.push(response);

	DEBUG_LOG
	return (0);
}
	
