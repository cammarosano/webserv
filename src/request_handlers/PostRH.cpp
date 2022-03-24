#include "PostRH.hpp"

#include <fcntl.h>
#include <sys/stat.h>

PostRH::PostRH(HttpRequest *request, FdManager &table)
    : AReqHandler(request, table), bd(*request) {
    Route *r = request->route;
    body = "{ \"success\" : \"true\" }";
    // check if upload_dir exist?
    struct stat sb;
    if (stat((r->root + '/' + r->upload_dir).c_str(), &sb) == -1) {
        // could be a custom error class with usefull infos
        throw std::exception();
    }
    std::string file_name =
        request->target.substr(r->prefix.length(), std::string::npos);
    file_path = r->root + '/' + r->upload_dir + '/' + file_name;
    state = s_start;

    if (request->header_fields.find("expect") != request->header_fields.end()) {
        response.http_version = "HTTP/1.1";
        response.status_code_phrase = "100 continue";
        assemble_header_str();
        state = s_send_100_continue;
    }
}

PostRH::~PostRH() {}

int PostRH::respond() {
    int ret_bd;

    if (state == s_send_100_continue) {
        if (send_header() == 1) state = s_start;
    }
    if (state == s_start) {
        fd = open(file_path.c_str(), O_CREAT | O_WRONLY, 0644);
        if (fd < 0) return 500;
        table.add_fd_write(fd, request->client);
        state = s_receiving_body;
    }
    if (state == s_receiving_body) {
        ret_bd = bd.decode_body();
        if (ret_bd == -1) return 400;
        Client &client = request->client;
        if (!client.decoded_body.empty()) table.set_pollout(fd);
        if (ret_bd == 0) return 0;
        state = s_sending_header;
    }
    if (state == s_sending_header) {
        response.http_version = "HTTP/1.1";
        response.status_code_phrase = "201 created";
        response.header_fields["content-length"] = long_to_str(body.length());
        assemble_header_str();
        if (send_header() == 1) state = s_sending_html_str;
    }
    if (state == s_sending_html_str) {
        if (send_html_str(body) == 1) state = s_done;
    }
    if (state == s_done) return 1;
    if (state == s_abort) return -1;
    return 0;
}

// close the opend fd ???
void PostRH::abort() { state = s_abort; }
