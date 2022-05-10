#ifndef STATICRH_HPP
#define STATICRH_HPP

#include "ARequestHandler.hpp"
#include "utils.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
Request handler for serving a static file.
*/
class StaticRH : public AReqHandler
{
  private:
	int fd_file;
	std::string resource_path;
	enum e_state
	{
		s_sending_header,
		s_sending_file,
		s_done,
	} state;

	int setup();

  public:
	StaticRH(HttpRequest *request, FdManager &table,
			 std::string &resource_path);
	~StaticRH();

	virtual int respond();
};

#endif
