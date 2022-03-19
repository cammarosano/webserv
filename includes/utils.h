#ifndef UTILS_H
# define UTILS_H

# include "HttpRequest.hpp"
# include <sstream>

// forward declaration
struct HttpRequest; // print_request debugging function

std::string long_to_str(long nb);
std::string str_tolower(std::string s);
void print_request(HttpRequest &request);
std::string &remove_trailing_spaces(std::string &s);
bool str_is_number(std::string &str);


#endif