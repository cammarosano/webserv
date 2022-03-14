#ifndef ERRORRH_HPP
#define ERRORRH_HPP

#include "ARequestHandler.hpp"
#include "includes.hpp"

#define DEFAULT_403_PAGE "error_pages/403.html"
#define DEFAULT_404_PAGE "error_pages/404.html"

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