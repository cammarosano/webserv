#include "DirectoryRH.hpp"

#include <ostream>

DirectoryRH::DirectoryRH(HttpRequest *request, FdManager &table,
                         const std::string &path)
    : ARequestHandler(request, table), ressource_path(path) {}

DirectoryRH::~DirectoryRH() {}

void DirectoryRH::_get_html_template() {}

int DirectoryRH::respond() {
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
    // res might be bigger than 4096
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\n";
    oss << "Content-Type: text/html; charset=UTF-8\n";
    oss << "Content-Length: " << res.str().size() << "\n\n";
    oss << res.str();
    request->client.unsent_data.append(oss.str());
    table.set_pollout(request->client.socket);
    return 1;
}

void DirectoryRH::abort() { state = s_abort; }
