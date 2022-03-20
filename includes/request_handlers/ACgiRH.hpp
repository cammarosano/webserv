#ifndef ACGIRH_HPP
# define ACGIRH_HPP

# include "ARequestHandler.hpp"
# include <unistd.h>
# include <string.h>
# include "utils.h"
# include <sys/types.h>
# include <sys/wait.h>

class ACgiRH : public ARequestHandler {
protected:
	std::string script_path;
	std::string query_str;
	pid_t pid_cgi_process;
	int	cgi_output_fd;
    bool child_exited;
    enum e_state // order matters!!
    {
        s_start,
        s_recv_req_body, s_sending_body2cgi, // CGI-POST only
        s_recving_cgi_output,
        s_wait_child,
        s_done,
        s_abort
    } state;

    std::string get_query_str();
	char **setup_cgi_argv();
	char **setup_cgi_env();
    bool is_cgi_error();
    int wait_child();
    void send_502_response();

private:
    std::map<std::string, std::string> get_env_map();

public:
    ACgiRH(HttpRequest *request, FdManager &table, std::string &script_path);
    ~ACgiRH();

    virtual int respond() = 0;
    virtual void abort() = 0;
};

#endif
