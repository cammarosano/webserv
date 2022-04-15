#ifndef ERRORRH_HPP
#define ERRORRH_HPP

# include "ARequestHandler.hpp"
# include "utils.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include "HttpResponse.hpp"

/*
Request handler for error responses
*/
class ErrorRH : public AReqHandler {
   private:
    int error_code;
    int fd;
    std::string html_page;
    enum { sending_default, sending_file } res_type;
    enum { s_setup, s_sending_header, s_sending_html_str,
        s_start_send_file, s_sending_file, s_done, s_abort} state;

    int setup();
    bool custom_error_page(std::string &err_page);
    bool look_up_err_page(std::map<int, std::string> &error_pages,
        std::string &file_name);

   public:
    ErrorRH(HttpRequest *request, FdManager &table, int error_code);
    ~ErrorRH();

    virtual int respond();
    virtual int time_out_code();
    
    static std::string generate_error_page(int error_code);
};

#endif