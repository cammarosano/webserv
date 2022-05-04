#if !defined(__DIRECTORY_RH_H__)
#define __DIRECTORY_RH_H__

#include "ARequestHandler.hpp"
#include "utils.h"
#include <cstddef>
#include <dirent.h>
#include <list>
#include <ostream>
#include <string>
#include <utility>

class DirectoryRH : public AReqHandler
{
  private:
	std::string ressource_path;
	std::string html_page;

	enum
	{
		s_setup,
		s_sending_header,
		s_sending_html_str,
		s_done,
		s_abort
	} state;

  public:
	DirectoryRH(HttpRequest *request, FdManager &table,
				const std::string &ressource_path);
	~DirectoryRH();

	virtual int respond();

  private:
	void _generate_autoindex_page();
	int _setup();
	std::list<std::pair<std::string, unsigned char> > _get_files_name();
};

#endif // __DIRECTORY_RH_H__
