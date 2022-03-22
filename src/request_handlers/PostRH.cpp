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

// int PostRH::_setup() {
//     if (request->header_fields.find("expect") !=
//     request->header_fields.end()) {
//         response.http_version = "HTTP/1.1";
//         response.status_code_phrase = "100 continue";
//         response.header_fields["content-length"] = "0";
//         assemble_header_str();
//         state = s_sending_header;
//         return 0;
//     }
//     // TODO:
//     response.http_version = "HTTP/1.1";
//     response.status_code_phrase = "200 OK";
//     state = s_sending_header;
//     return 2;
// }

int PostRH::respond() {
    Route *r = request->route;
    std::string file_name =
        request->target.substr(r->prefix.length(), std::string::npos);
    std::string upload_path = r->root + '/' + r->upload_dir + '/' + file_name;
    std::cout << "upload_path: " << upload_path << std::endl;
    if (state == s_start) {
        fd = open(upload_path.c_str(), O_CREAT | O_WRONLY, 0644);
        if (fd < 0) {
            std::cerr << "failed\n";
            return 500;
        }
        table.add_cgi_in_fd(fd, request->client);
        state = s_receiving_body;
    }
    if (state == s_receiving_body) {
        int ret = bd.decode_body();
        // there's data to write to fd
        table.set_pollout(fd);
        if (ret == 0) return 0;
        state = s_done;
        return 1;
    }
    return 0;
    /*if (state == s_setup) {
        if (request->header_fields.find("expect") !=
            request->header_fields.end()) {
            response.http_version = "HTTP/1.1";
            response.status_code_phrase = "100 continue";
            assemble_header_str();
            state = s_sending_header;
        } else {
            // TODO:
            response.http_version = "HTTP/1.1";
            response.status_code_phrase = "200 OK";
            state = s_sending_header;
        }
    }
    if (state == s_sending_header) {
        if (send_header() == 1) state = s_wait_body;
        return 0;
    }
    if (state == s_wait_body) {
        if (request->client.received_data.empty()) return 0;
        size_t body_len =
            std::atoi(request->header_fields["content-length"].c_str());
        rcv_data_size += request->client.received_data.length();
        if (rcv_data_size < body_len) {
            buffer += request->client.received_data;
            request->client.received_data.erase(0, body_len);
            return 0;
        }
        buffer += request->client.received_data;
        request->client.received_data.erase(0, std::string::npos);
        _save_file();
        state = s_done;
        return 1;
    }*/
}

void PostRH::abort() { state = s_abort; }