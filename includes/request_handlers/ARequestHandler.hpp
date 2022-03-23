#ifndef AREQUESTHANDLER_HPP
#define AREQUESTHANDLER_HPP

#include <ctime>

#include "FdManager.hpp"
#include "HttpRequest.hpp"
#include "macros.h"
#include "utils.h"

/*
Abstract class for request handlers
Request handlers for a specific kind of response (ex: serve a static file)
inherit from this class and define the respond() and abort() methods
*/
class AReqHandler {
   protected:
    HttpRequest *request;
    Client &client;
    FdManager &table;
    std::string header_str;
    time_t last_io_activity;
    bool client_disconnected;
    size_t bytes_sent;

    enum e_rhstate {
        s_setup,
        s_sending_header,
        s_start_send_file,
        s_sending_file,
        s_done,
        s_abort,
        s_sending_html_str
    } state;

    struct HttpResponse {
        std::string http_version;
        std::string status_code_phrase;
        std::map<std::string, std::string> header_fields;
    } response;

    void assemble_header_str();
    int send_header();
    int send_html_str(std::string &html_page);
    bool response100_expected();


   public:
    AReqHandler(HttpRequest *request, FdManager &table);
    virtual ~AReqHandler();

    virtual int respond() = 0;
    virtual void abort() = 0;
    virtual int time_out_abort();

    bool is_time_out();
    HttpRequest *getRequest();
    void update_last_io_activ();
    void lock_client();
    void unlock_client();
    void disconnect_client();
    void add_to_bytes_sent(size_t n);

	// maps extensions to content-type
    static std::map<std::string, std::string> content_type;
};

#endif
