#ifndef CGIPOSTRH
# define CGIPOSTRH

# include "ACgiRH.hpp"
# include "BodyDecoder.hpp"

class CgiPostRH: public ACgiRH
{
private:
	int cgi_input_fd;
	BodyDecoder bd;

	int setup();
	void release_resources();

public:
	CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path);
	~CgiPostRH();

	virtual int respond();
	virtual void abort();
};

#endif