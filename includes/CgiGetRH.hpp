#ifndef CGIGETRH
# define CGIGETRH

# include "ACgiRH.hpp"
# include "includes.hpp"

class CgiGetRH: public ACgiRH
{

private:
	int setup();

public:
	CgiGetRH(HttpRequest *request, FdManager &table,
			std::string &script_path);
	~CgiGetRH();

	virtual int respond();
	virtual void abort();
};


#endif