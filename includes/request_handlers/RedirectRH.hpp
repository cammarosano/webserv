#if !defined(__REDIRECT_RH_HPP__)
#define __REDIRECT_RH_HPP__

#include "ARequestHandler.hpp"
#include "utils.h"

class RedirectRH : public AReqHandler
{
  public:
	RedirectRH(HttpRequest *request, FdManager &table);
	~RedirectRH();

	virtual int respond();
};

#endif // __REDIRECT_RH_HPP__
