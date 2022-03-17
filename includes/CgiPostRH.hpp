#ifndef CGIPOSTRH
# define CGIPOSTRH

# include "ACgiRH.hpp"
# include "includes.hpp"
# include "BodyDecoder.hpp"

class CgiPostRH: public ACgiRH
{
private:
	int cgi_input_fd;
	BodyDecoder bd;

	int setup();

public:
	CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query);
	~CgiPostRH();

	virtual int respond();
	virtual void abort();
};

#endif