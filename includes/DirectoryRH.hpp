#if !defined(__DIRECTORY_RH_H__)
#define __DIRECTORY_RH_H__

#include "ARequestHandler.hpp"

class DirectoryRH : public ARequestHandler {
   private:
   public:
    DirectoryRH(/* args */);
    ~DirectoryRH();

    virtual int respond();
    virtual void abort();
};

#endif  // __DIRECTORY_RH_H__
