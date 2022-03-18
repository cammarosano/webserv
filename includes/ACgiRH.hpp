#ifndef __A_CGI_RH__
#define __A_CGI_RH__

#include "ARequestHandler.hpp"

class ACgiRH : public ARequestHandler {
protected:
	std::string script_path;
	std::string query_str;
	pid_t pid_cgi_process;
	int	cgi_output_fd;
    enum e_state
    {
        st_setup,
        st_recv_req_body, st_sending_body2cgi, st_recving_cgi_output,
        st_done,
        st_abort
    } state;

    std::string get_query_str();
	char **setup_cgi_argv();
	char **setup_cgi_env();
	void clear_resources();

private:
    std::map<std::string, std::string> get_env_map();

public:
    ACgiRH(HttpRequest *request, FdManager &table, std::string &script_path);
    ~ACgiRH();

    virtual int respond() = 0;
    virtual void abort() = 0;
};

#endif  // !__A_CGI_RH__
