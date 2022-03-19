#ifndef STATICRH_HPP
#define STATICRH_HPP

# include "ARequestHandler.hpp"
# include "utils.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>

/*
Request handler for serving a static file.
*/
class StaticRH : public ARequestHandler {
   private:
    int fd_file;
    std::string resource_path;

    int setup();

   public:
    StaticRH(HttpRequest *request, FdManager &table,
             std::string &resource_path);
    ~StaticRH();

    virtual int respond();
    virtual void abort();
};

#endif
