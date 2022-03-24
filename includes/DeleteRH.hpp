#ifndef __DELETE_RH_HPP__

#include <ostream>

#include "ARequestHandler.hpp"

class DeleteRH : public AReqHandler {
   public:
    DeleteRH(HttpRequest *request, FdManager &table,
             std::string &ressource_path);
    ~DeleteRH();

   private:
    std::string ressource_path;
    std::string body;

    virtual int respond();
    virtual void abort();

    int setup();
};

#endif  // !__DELETE_RH_HPP__
