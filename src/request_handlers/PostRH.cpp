#include "PostRH.hpp"

#include <fcntl.h>

#include <fstream>

PostRH::PostRH(HttpRequest *request, FdManager &table)
    : ARequestHandler(request, table), bd(*request), rcv_data_size(0) {
    if (request->header_fields.find("expect") != request->header_fields.end()) {
        response.http_version = "HTTP/1.1";
        response.status_code_phrase = "100 continue";
        assemble_header_str();
        if (send_header() == 1) state = s_start;
    }
}

PostRH::~PostRH() {}

int PostRH::_save_file() {
    Route *r = request->route;
    std::string file_name =
        request->target.substr(r->prefix.length(), std::string::npos);
    std::string upload_path = r->root + '/' + r->upload_dir + '/' + file_name;
    std::ofstream fs(upload_path.c_str());
    if (!fs.good()) return 0;
    fs << buffer;
    fs.close();
    return 1;
}

int PostRH::respond() {
    Route *r = request->route;
    int ret_bd;
    std::string body = "{ \"success\" : \"true\" }";
    std::string file_name =
        request->target.substr(r->prefix.length(), std::string::npos);
    std::string upload_path = r->root + '/' + r->upload_dir + '/' + file_name;
    std::cout << "upload_path: " << upload_path << std::endl;
    if (state == s_start) {
        fd = open(upload_path.c_str(), O_CREAT | O_WRONLY, 0644);
        if (fd < 0) return 500;
        table.add_cgi_in_fd(fd, request->client);
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
    return 0;
}

void PostRH::abort() { state = s_abort; }