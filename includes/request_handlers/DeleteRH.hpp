#ifndef __DELETE_RH_HPP__

#include <ostream>

#include "ARequestHandler.hpp"

class DeleteRH : public AReqHandler
{
  public:
    DeleteRH(HttpRequest *request, FdManager &table, std::string &ressource_path);
    ~DeleteRH();

  private:
    enum
    {
        s_setup,
        s_sending_header,
        s_sending_html_str,
        s_done,
        s_abort
    } state;
    std::string ressource_path;
    std::string body;

    virtual int respond();

    int setup();
};

#endif // !__DELETE_RH_HPP__
