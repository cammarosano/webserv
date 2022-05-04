#ifndef CGIGETRH
#define CGIGETRH

#include "ACgiRH.hpp"
#include <stdlib.h>

class CgiGetRH : public ACgiRH
{

  private:
	int setup();

  public:
	CgiGetRH(HttpRequest *request, FdManager &table, std::string &script_path);
	~CgiGetRH();

	virtual int respond();
	virtual int time_out_code();
};

#endif