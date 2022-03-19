#ifndef ERRORRH_HPP
#define ERRORRH_HPP

# include "ARequestHandler.hpp"
# include "utils.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>


/*
Request handler for error responses
*/
class ErrorRH : public ARequestHandler {
   private:
    int error_code;
    int fd;
    std::string html_page;
    enum { sending_default, sending_file } res_type;

    int setup();
    void generate_error_page();
    int send_html_str();

    static std::map<int, std::string> reason_phrases;

   public:
    ErrorRH(HttpRequest *request, FdManager &table, int error_code);
    ~ErrorRH();

    virtual int respond();
    virtual void abort();

    static std::map<int, std::string> init_map();
};

#endif