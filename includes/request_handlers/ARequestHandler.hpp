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
    time_t last_io_activity;
    bool client_disconnected;
    size_t bytes_sent;

    struct HttpResponse {
        std::string http_version;
        std::string status_code_phrase;
        std::map<std::string, std::string> header_fields;

        std::string header_str; // assemble_header_str() fills this up
        void assemble_header_str();
    } response;

    int send_str(std::string &str);
    bool response100_expected();

    std::string get_mime_type(const std::string &file_name) const;

   public:
    AReqHandler(HttpRequest *request, FdManager &table);
    virtual ~AReqHandler();

    virtual int respond() = 0;
    virtual void abort() = 0;
    virtual int time_out_abort();

    bool is_time_out(time_t current_time);
    HttpRequest *getRequest();
    Client *getClient();
    void set_last_io_activ(time_t current_time);
    void add_to_bytes_sent(size_t n);

	// maps extensions to content-type
    static std::map<std::string, std::string> content_type;
};

#endif
