#include "DeleteRH.hpp"

DeleteRH::DeleteRH(HttpRequest *request, FdManager &table,
                   std::string &ressource_path)
    : AReqHandler(request, table), ressource_path(ressource_path) {
    state = s_setup;
    setup();
}

DeleteRH::~DeleteRH() {}

int DeleteRH::setup() {
    std::ostringstream temp;

    temp << "<html> <body><h1>ressource deleted</h1></body> </html>";
    if (remove(ressource_path.c_str()) == -1) {
        // TODO
        throw std::exception();
    }
    response.http_version = "HTTP/1.1";
    response.status_code_phrase = "200 OK";
    response.header_fields["content-length"] = long_to_str(temp.str().length());
    assemble_header_str();
    state = s_sending_header;
    this->body = temp.str();
    return 0;
}

int DeleteRH::respond() {
    if (state == s_sending_header) {
        if (send_header() == 1) state = s_sending_html_str;
    }
    if (state == s_sending_html_str) {
        if (send_html_str(body) == 1) {
            state = s_done;
        }
    }
    if (state == s_done) return 1;
    if (state == s_abort) return -1;
    return 0;
}

void DeleteRH::abort() { state = s_abort; }
