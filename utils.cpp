#include "includes.hpp"

std::string long_to_str(long nb)
{
	std::ostringstream ss;
	ss << nb;
	return (ss.str());
}