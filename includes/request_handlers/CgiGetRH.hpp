#ifndef CGIGETRH
# define CGIGETRH

# include "ACgiRH.hpp"
# include <stdlib.h> // exit

class CgiGetRH: public ACgiRH
{

private:
	int setup();
	void release_resources();

public:
	CgiGetRH(HttpRequest *request, FdManager &table,
			std::string &script_path);
	~CgiGetRH();

	virtual int respond();
	virtual void abort();
};


#endif