#include "ErrorRH.hpp"

ErrorRH::ErrorRH(HttpRequest *request, FdManager &table, int error_code)
	: AReqHandler(request, table), error_code(error_code)
{
	state = s_setup;
	table.unset_pollin(client.socket); // client will be disconnected after resp
	client.received_data.clear();	   // clear possible body of a bad request
	keep_alive = false;
	response.header_fields["Connection"] = "close";
}

ErrorRH::~ErrorRH()
{
	if (res_type == sending_file)
	{
		close(fd);
		table.remove_fd(fd);
	}
}

// true if found, and puts it in file_name
bool ErrorRH::look_up_err_page(std::map<int, std::string> &error_pages,
							   std::string &file_name)
{
	std::map<int, std::string>::iterator it = error_pages.find(error_code);

	if (it == error_pages.end()) // not found
		return (false);
	file_name = it->second;
	return (true);
}

// returns true if there's a custom page and fills in err_page
// false otherwise
bool ErrorRH::custom_error_page(std::string &err_page)
{
	// check if route has custom page
	if (request->route &&
		look_up_err_page(request->route->error_pages, err_page))
		return (true);
	// check if vserver has custom page
	else if (look_up_err_page(request->vserver->err_pages, err_page))
		return (true);
	return (false);
}

// resolves response type (default or custom page)
// opens fd || generates default page
// assembles header_str
int ErrorRH::setup()
{
	struct stat sb;
	std::string err_page;

	// resolve res_type
	res_type = sending_default;
	if (custom_error_page(err_page))
	{
		fd = open(err_page.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd != -1)
		{
			if (fstat(fd, &sb) == 0) // if no error
				res_type = sending_file;
			else
				close(fd);
		}
	}

	// generate default-page
	if (res_type == sending_default)
		html_page = generate_error_page(error_code);

	// fill-in header
	response.status_code = error_code;
	if (res_type == sending_default)
	{
		response.header_fields["Content-Length"] =
			long_to_str(html_page.size());
		response.header_fields["Content-Type"] = "text/html";
	}
	else
	{
		response.header_fields["Content-Length"] = long_to_str(sb.st_size);
		response.header_fields["Content-Type"] =
			response.get_mime_type(err_page);
	}
	if (error_code == 405)
		response.include_allow_header(request->route->accepted_methods);

	response.assemble_header_str();
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
		state = s_sending_header;
	}
	if (state == s_sending_header)
	{
		if (send_str(response.header_str) == 1)
		{
			if (res_type == sending_default)
				state = s_sending_html_str;
			else
				state = s_start_send_file;
		}
	}
	if (state == s_sending_html_str)
	{
		if (send_str(html_page) == 1)
			state = s_done;
	}
	else if (state == s_start_send_file)
	{
		table.add_fd_read(fd, client);
		state = s_sending_file;
		return 0;
	}
	if (state == s_sending_file)
	{
		if (table[fd].is_EOF)
			state = s_done;
	}
	if (state == s_done)
		return (1);
	return (0);
}

int ErrorRH::time_out_code()
{
	return (0); // means: no error response to inform this time, as this already
				// is an error response
}

// static function
std::string ErrorRH::generate_error_page(int error_code)
{
	std::ostringstream oss;
	std::string error_reason = HttpResponse::reason_phrases[error_code];

	oss << "<!DOCTYPE html>"
		<< "<html>"
		<< "<head>"
		<< "<title>" << error_code << ' ' << error_reason << "</title>"
		<< "</head>"
		<< "<body>"
		<< "<p>"
		<< "Webserv42 default error page"
		<< "</p>"
		<< "<h1>" << error_code << ' ' << error_reason << "</h1>"
		<< "</body>"
		<< "</html>";

	return (oss.str());
}
