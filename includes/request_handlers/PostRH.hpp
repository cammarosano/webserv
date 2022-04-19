#if !defined(__POST_RH_HPP__)
#define __POST_RH_HPP__

#include "ARequestHandler.hpp"
#include "BodyDecoder.hpp"
#include "macros.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

class PostRH : public AReqHandler
{
  private:
    BodyDecoder bd;
    std::string file_path;
    std::string body;

    int fd;

    enum
    {
        s_send_100_continue,
        s_start,
        s_receiving_body,
        s_assemble_header,
        s_sending_header,
        s_sending_html_str,
        s_done,
    } state;

  public:
    PostRH(HttpRequest *request, FdManager &table);
    ~PostRH();

    virtual int respond();
};

#endif // __POST_RH_HPP__
