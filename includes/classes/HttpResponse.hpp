#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "macros.h"
#include "utils.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

struct HttpResponse
{
	std::string http_version;
	int status_code;
	std::map<std::string, std::string> header_fields;

	std::string header_str; // assemble_header_str() fills this up
	void assemble_header_str();
	void assemble_partial_header_str();
	void assemble_100_continue_str();
	std::string get_mime_type(const std::string &file_name) const;
	void include_allow_header(std::list<std::string> &accepted_methods);

	// maps extensions to content-type
	static std::map<std::string, std::string> content_type;
	// maps status codes to reason phrases
	static std::map<int, std::string> reason_phrases;
	static std::map<int, std::string> init_map();

  private:
	std::string get_timestamp();
};

#endif