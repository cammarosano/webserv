#include "includes.hpp"

std::string long_to_str(long nb)
{
	std::ostringstream ss;
	ss << nb;
	return (ss.str());
}

// changes s in place
std::string & str_tolower(std::string &s)
{
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
		*it = tolower(*it);
	return (s);
}

// for debugging
void print_request(HttpRequest &request)
{
	std::cout << "method: " << request.method << '\n'
			<< "target: " << request.request_target << '\n'
			<< "http-version: " << request.http_version << '\n';

	for (std::map<std::string, std::string>::iterator it = request.header_fields.begin();
			it != request.header_fields.end(); it++)
		std::cout << it->first << ":" << it->second << '\n';
		
}