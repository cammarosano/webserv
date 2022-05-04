#include "utils.h"

std::string long_to_str(long nb)
{
	std::ostringstream ss;
	ss << nb;
	return (ss.str());
}

std::string str_tolower(std::string s)
{
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
		*it = tolower(*it);
	return (s);
}

// changes s in place
std::string &remove_trailing_spaces(std::string &s)
{
	size_t i;

	i = 0;
	while (i < s.length() && isspace(s[i]))
		++i;
	s.erase(0, i);
	return (s);
}

bool str_is_number(std::string &str)
{
	std::string::const_iterator it = str.begin();

	for (; it != str.end(); ++it)
	{
		if (!std::isdigit(*it))
			return false;
	}
	return true;
}
