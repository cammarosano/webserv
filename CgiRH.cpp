#include "CgiRH.hpp"

#include <ostream>

CgiRH::CgiRH(HttpRequest *request, FdManager &table)
    : ARequestHandler(request, table) {}

CgiRH::~CgiRH() {}

int CgiRH::respond() {
    std::ostringstream oss;
    std::string res = "<h1> cgi route </h1>";

    oss << "HTTP/1.1 200 OK\n";
    oss << "Content-Type: text/html; charset=UTF-8\n";
    oss << "Content-Length: " << res.length() << "\n\n";
    oss << res;
    request->client.unsent_data.append(oss.str());
    table.set_pollout(request->client.socket);
    return 1;
}

void CgiRH::abort() {}