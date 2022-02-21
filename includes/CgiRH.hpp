#ifndef __CGI_RH__

#include "ARequestHandler.hpp"

class CgiRH : public ARequestHandler {
   private:
   public:
    CgiRH(HttpRequest *request, FdManager &table);
    ~CgiRH();

    virtual int respond();
    virtual void abort();
};

#endif  // !__CGI_RH__
