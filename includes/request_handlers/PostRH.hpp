#if !defined(__POST_RH_HPP__)
#define __POST_RH_HPP__

#include "ARequestHandler.hpp"
#include "BodyDecoder.hpp"

class PostRH : public ARequestHandler {
   private:
    BodyDecoder bd;
    std::string buffer;
    size_t rcv_data_size;
    int fd;

    enum { s_start, s_receiving_body, s_done, s_abort } state;

    int _save_file();
    int _setup();

   public:
    PostRH(HttpRequest *request, FdManager &table);
    ~PostRH();

    virtual int respond();
    virtual void abort();
};

#endif  // __POST_RH_HPP__