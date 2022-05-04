#ifndef CGIPOSTRH
#define CGIPOSTRH

#include "ACgiRH.hpp"
#include "BodyDecoder.hpp"

class CgiPostRH : public ACgiRH
{
  private:
	int cgi_input_fd;
	BodyDecoder bd;
	bool limit_body;
	size_t max_body_size;

	int setup();

  public:
	CgiPostRH(HttpRequest *request, FdManager &table, std::string &script_path);
	~CgiPostRH();

	virtual int respond();
	virtual int time_out_code();
};

#endif