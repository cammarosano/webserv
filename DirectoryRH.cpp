#include "DirectoryRH.hpp"

#include <ostream>

DirectoryRH::DirectoryRH(HttpRequest *request, FdManager &table,
                         const std::string &path)
    : ARequestHandler(request, table), ressource_path(path) {
    state = s_setup;
}

DirectoryRH::~DirectoryRH() {}

void DirectoryRH::_generate_autoindex_page() {
    DIR *d;
    std::ostringstream res;
    struct dirent *dir;

    d = opendir(ressource_path.c_str());
    res << "<!DOCTYPE html>\n";
    res << "<html>\n";
    res << "<head>\n";
    res << "<title>" << ressource_path << "</title>\n";
    res << "</head>\n";
    res << "<body>\n";
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string param = request->target;
            std::string::iterator it = --(param.end());
            if (*it == '/') param.erase(it);
            if (param == "/") param.clear();
            res << "<a href="
                << "\"" << param << "/" << dir->d_name << "\" >" << dir->d_name;
            if (dir->d_type == DT_DIR) res << "/";
            res << "</a> <br>";
        }
        closedir(d);
    }
    res << "</body>\n";
    res << "</html>";
    html_page = res.str();
}

int DirectoryRH::_send_html_str() {
    Client &client = request->client;
    int max_bytes;

    max_bytes = BUFFER_SIZE - client.unsent_data.size();
    if (max_bytes <= 0) return 0;

    client.unsent_data += html_page.substr(0, max_bytes);
    html_page.erase(0, max_bytes);
    table.set_pollout(client.socket);
    if (html_page.empty()) return 1;
    return 0;
}

int DirectoryRH::_setup() {
    _generate_autoindex_page();

    response.http_version = "HTTP/1.1";
    response.status_code_phrase = "200 OK";
    response.header_fields["content-length"] = long_to_str(html_page.length());

    assemble_header_str();
    return 0;
}

int DirectoryRH::respond() {
    if (state == s_setup) {
        _setup();
        state = s_sending_header;
    }
    if (state == s_sending_header) {
        if (send_header() == 1) state = s_sending_html_str;
    }
    if (state == s_sending_html_str) {
        if (_send_html_str() == 1) state = s_done;
    }
    if (state == s_done) return 1;
    if (state == s_abort) return -1;
    return 0;
}

void DirectoryRH::abort() { state = s_abort; }
