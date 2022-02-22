#if !defined(__DIRECTORY_RH_H__)
#define __DIRECTORY_RH_H__

#include "ARequestHandler.hpp"
#include <dirent.h>

class DirectoryRH : public ARequestHandler {
   private:
    std::string ressource_path;

   public:
    DirectoryRH(HttpRequest *request, FdManager &table,
                const std::string &ressource_path);
    ~DirectoryRH();

    virtual int respond();
    virtual void abort();
};

#endif  // __DIRECTORY_RH_H__
