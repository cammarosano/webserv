#include "HttpResponse.hpp"

void HttpResponse::assemble_partial_header_str()
{
	typedef std::map<std::string, std::string>::iterator iterator;

	// default-additions
	if (http_version.empty())
		http_version = "HTTP/1.1";
	header_fields["Server"] = "Webserv";
	header_fields["Date"] = get_timestamp();

	// generate status-line
	header_str = http_version + ' ' + long_to_str(status_code) + ' ' +
				 reason_phrases[status_code] + "\r\n";
	// log
	std::cout << "Response: " << header_str;

	// add header-fiels
	for (iterator it = header_fields.begin(); it != header_fields.end(); ++it)
		header_str += it->first + ": " + it->second + "\r\n";
}

void HttpResponse::assemble_header_str()
{
	assemble_partial_header_str();
	// end header
	header_str += "\r\n";
}

void HttpResponse::assemble_100_continue_str()
{
	header_str = "HTTP/1.1 100 Continue\r\n\r\n";
}

std::string get_extension(const std::string &file_name)
{
	size_t pos = file_name.rfind('.');
	std::string extension;

	// if found and not the last char
	if (pos != std::string::npos && pos < file_name.size() - 1)
		extension = file_name.substr(pos + 1);
	return (extension);
}

// returns the mime_type based on file_name's extension
std::string HttpResponse::get_mime_type(const std::string &file_name) const
{
	std::map<std::string, std::string>::iterator it;
	std::string ext = get_extension(file_name);

	if (ext.empty()) // no extension
		return (DEFAULT_MIME);
	if (ext == "html") // optimization: avoid a map look-up
		return ("text/html");
	it = content_type.find(ext);
	if (it == content_type.end()) // ext not found
		return (DEFAULT_MIME);
	return (it->second);
}

// timestamp for "date" header-field
std::string HttpResponse::get_timestamp()
{
	static const char *wday_name[] = {"Sun", "Mon", "Tue", "Wed",
									  "Thu", "Fri", "Sat"};
	static const char *mon_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
									 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	time_t now = time(NULL);
	tm *t = gmtime(&now);
	std::ostringstream timestamp;
	timestamp << std::setfill('0');
	timestamp << wday_name[t->tm_wday] << ", ";
	timestamp << std::setw(2) << t->tm_mday << ' ';
	timestamp << mon_name[t->tm_mon] << ' ';
	timestamp << 1900 + t->tm_year << ' ';
	timestamp << std::setw(2) << t->tm_hour << ':';
	timestamp << std::setw(2) << t->tm_min << ':';
	timestamp << std::setw(2) << t->tm_sec << ' ';
	timestamp << "GMT";
	return (timestamp.str());
}

void HttpResponse::include_allow_header(
	std::list<std::string> &accepted_methods)
{
	if (accepted_methods.empty())
		header_fields["Allow"] = "GET, HEAD";
	else
	{
		std::list<std::string>::iterator it = accepted_methods.begin();
		header_fields["Allow"] = *it++;
		while (it != accepted_methods.end())
			header_fields["Allow"] += ", " + *it++;
	}
}

// static variable
std::map<std::string, std::string> HttpResponse::content_type;

// static function
std::map<int, std::string> HttpResponse::init_map()
{
	std::map<int, std::string> map;

	map[200] = "OK";
	map[201] = "Created";

	map[301] = "Moved Permanently";
	map[302] = "Found";
	map[303] = "See Other";
	map[304] = "Not Modified";
	map[307] = "Temporary Redirect";
	map[308] = "Permanent Redirect";

	map[400] = "Bad Request";
	map[403] = "Forbidden";
	map[404] = "Not Found";
	map[405] = "Method Not Allowed";
	map[408] = "Request Timeout";
	map[413] = "Payload Too Large";
	map[431] = "Request Header Fields Too Large";

	map[500] = "Internal Server Error";
	map[501] = "Not Implemented";
	map[502] = "Bad Gateway";
	map[504] = "Gateway Timeout";

	return map;
}

// static variable
std::map<int, std::string> HttpResponse::reason_phrases = init_map();
