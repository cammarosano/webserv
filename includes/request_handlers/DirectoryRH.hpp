#if !defined(__DIRECTORY_RH_H__)
#define __DIRECTORY_RH_H__

#include <dirent.h>

#include <ostream>
#include "utils.h"

#include "ARequestHandler.hpp"

class DirectoryRH : public ARequestHandler {
   private:
    std::string ressource_path;
    std::string html_page;

   public:
    DirectoryRH(HttpRequest *request, FdManager &table,
                const std::string &ressource_path);
    ~DirectoryRH();

    virtual int respond();
    virtual void abort();

   private:
    void _generate_autoindex_page();
    int _send_html_str();
    int _setup();
};

#endif  // __DIRECTORY_RH_H__
