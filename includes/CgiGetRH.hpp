#ifndef CGIGETRH
# define CGIGETRH

# include "ARequestHandler.hpp"
# include "includes.hpp"

class CgiGetRH: public ARequestHandler
{
protected:
	std::string script_path;
	std::string query;
	pid_t pid_cgi_process;
	int	cgi_output_fd;

	void setup_cgi_argv(char **argv);
	void setup_cgi_env(char **envp);

private:
	int setup();

public:
	CgiGetRH(HttpRequest *request, FdManager &table,
			std::string &script_path, std::string &query);
	~CgiGetRH();

	virtual int respond();
	virtual void abort();
};


#endif