#ifndef __CGI_RH__
#define __CGI_RH__

#include "ARequestHandler.hpp"

class CgiRH : public ARequestHandler {
   private:
    void _setup_cgi_env();
    char *env[20];

   public:
    CgiRH(HttpRequest *request, FdManager &table);
    ~CgiRH();

    virtual int respond();
    virtual void abort();
};

#endif  // !__CGI_RH__
