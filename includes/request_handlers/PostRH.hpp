#if !defined(__POST_RH_HPP__)
#define __POST_RH_HPP__

#include "ARequestHandler.hpp"
#include "BodyDecoder.hpp"
class PostRH : public AReqHandler {
   private:
    BodyDecoder bd;
    std::string file_path;
    std::string body;
    int fd;

    enum {
        s_start,
        s_receiving_body,
        s_done,
        s_abort,
        s_send_100_continue,
        s_sending_header,
        s_sending_html_str
    } state;

    int _save_file();
    int _setup();

   public:
    PostRH(HttpRequest *request, FdManager &table);
    ~PostRH();

    virtual int respond();
    virtual void abort();
};

#endif  // __POST_RH_HPP__
