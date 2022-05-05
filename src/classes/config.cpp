#include "config.hpp"

Route::Route(std::string prefix)
	: prefix(prefix), auto_index(false), redirected(false), body_size_limit(0)
{
}

Vserver::Vserver() : redirected(false)
{
}