#ifndef UTILS_H
#define UTILS_H

#include <sstream>

std::string long_to_str(long nb);
std::string str_tolower(std::string s);
std::string &remove_trailing_spaces(std::string &s);
bool str_is_number(std::string &str);

#endif