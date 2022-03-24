#include "RedirectRH.hpp"

RedirectRH::RedirectRH(HttpRequest *request, FdManager &table)
    : AReqHandler(request, table) {
    Redirection redirect;

    if (request->vserver->redirected)
        redirect = request->vserver->redirect;
    else
        redirect = request->route->redirect;
    response.http_version = "HTTP/1.1";
    response.status_code_phrase = long_to_str(redirect.status_code) + ' ' +
                                  reason_phrases[redirect.status_code];
    response.header_fields["location"] = redirect.location;
    assemble_header_str();
}

RedirectRH::~RedirectRH() {}

int RedirectRH::respond() {
    if (send_header() == 0)
        return 0;
    else
        state = s_done;
    return 1;
}

void RedirectRH::abort() { state = s_abort; }

std::map<int, std::string> RedirectRH::init_map() {
    std::map<int, std::string> map;

    map[301] = "Moved Permanently";
    map[302] = "Found";
    map[303] = "See Other";
    map[304] = "Not Modified";
    map[307] = "Temporary Redirect";
    map[308] = "Permanent Redirect";
    return map;
}

// static variable
std::map<int, std::string> RedirectRH::reason_phrases = init_map();