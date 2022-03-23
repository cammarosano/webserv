#if !defined(__REDIRECT_RH_HPP__)
#define __REDIRECT_RH_HPP__

#include "ARequestHandler.hpp"
#include "utils.h"

class RedirectRH : public AReqHandler {
   private:
    static std::map<int, std::string> reason_phrases;

   public:
    RedirectRH(HttpRequest *request, FdManager &table);
    ~RedirectRH();

    virtual int respond();
    virtual void abort();

    static std::map<int, std::string> init_map();
};

#endif  // __REDIRECT_RH_HPP__
