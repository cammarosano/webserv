#include "DirectoryRH.hpp"

#include <ostream>

#include "ARequestHandler.hpp"

DirectoryRH::DirectoryRH(HttpRequest *request, FdManager &table,
                         const std::string &path)
    : ARequestHandler(request, table), ressource_path(path) {}

DirectoryRH::~DirectoryRH() {}

int DirectoryRH::respond() {
    DIR *d;
    std::ostringstream res;
    struct dirent *dir;

    d = opendir(ressource_path.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (*dir->d_name == '.') continue;
            res << "<a href="
                << "\"" << request->target << "/" << dir->d_name << "\" >"
                << dir->d_name;
            if (dir->d_type == DT_DIR) res << "/";
            res << "</a> <br>";
        }
        closedir(d);
    }
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

void DirectoryRH::abort() {}
