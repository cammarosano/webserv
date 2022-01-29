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
