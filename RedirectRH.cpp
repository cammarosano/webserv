#include "RedirectRH.hpp"

RedirectRH::RedirectRH(HttpRequest *request, FdManager &table)
    : ARequestHandler(request, table) {}

RedirectRH::~RedirectRH() {}

int RedirectRH::respond() {
    Redirection redirect;

    if (request->route->redirected)
        redirect = request->route->redirect;
    else
        redirect = request->vserver->redirect;
    response.http_version = "HTTP/1.1";
    response.status_code_phrase = long_to_str(redirect.status_code) + ' ' +
                                  reason_phrases[redirect.status_code];
    response.header_fields["location"] = redirect.location;
    assemble_header_str();
    if (send_header() == 1) {
        state = s_done;
        return 1;
    }
    state = s_abort;
    return -1;
}

void RedirectRH::abort() { state = s_abort; }

std::map<int, std::string> RedirectRH::init_map() {
    std::map<int, std::string> map;

    map[301] = "Moved Permanently";
    map[302] = "Found";
    map[308] = "Permanent Redirect";
    map[307] = "Temporary Redirect";
    return map;
}

// static variable
std::map<int, std::string> RedirectRH::reason_phrases = init_map();