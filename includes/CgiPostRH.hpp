#ifndef CGIPOSTRH
# define CGIPOSTRH

# include "ACgiRH.hpp"
# include "includes.hpp"

class CgiPostRH: public ACgiRH
{
private:
	int cgi_input_fd;
	int body_len_left;

	int setup();
	int get_body_len();
	int send_body2cgi();

public:
	CgiPostRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query);
	~CgiPostRH();

	virtual int respond();
	virtual void abort();
};

#endif