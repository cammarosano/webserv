#include "config.hpp"

Route::Route(std::string prefix)
	: prefix(prefix), auto_index(false), redirected(false), body_size_limit(0)
{
}

// returns false if prefix or root is empty
bool Route::validate()
{
	if (prefix.empty() || root.empty())
		return false;
	return true;
}

// adds '/' to end of prefix
// removes '/'s from end of root
void Route::sanitize()
{
	if (!validate())
		return;
	if (*prefix.rbegin() != '/')
		prefix += '/';
	while (!root.empty() && *root.rbegin() == '/')
		root.erase(root.size() - 1);
}

Vserver::Vserver() : listen(std::make_pair("127.0.0.1", 80)), redirected(false)
{
}
