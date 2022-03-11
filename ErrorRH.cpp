#include "ErrorRH.hpp"

ErrorRH::ErrorRH(HttpRequest *request, FdManager &table, int error_code)
    : ARequestHandler(request, table), error_code(error_code) {
    state = s_setup;
    res_type = sending_default;
}

ErrorRH::~ErrorRH() {}

void ErrorRH::generate_error_page() {
    std::ostringstream oss;

    oss << "<!DOCTYPE html>"
        << "<html>"
        << "<head>"
        << "<title>" << error_code << ' ' << reason_phrases[error_code]
        << "</title>"
        << "</head>"
        << "<body>"
        << "<p>"
        << "Webserv42 default error page"
        << "</p>"
        << "<h1>" << error_code << ' ' << reason_phrases[error_code] << "</h1>"
        << "</body>"
        << "</html>";

    html_page = oss.str();
}

// ! repeaded code (see send_header)
int ErrorRH::send_html_str() {
    Client &client = request->client;
    int max_bytes;

    max_bytes = BUFFER_SIZE - client.unsent_data.size();
    if (max_bytes <= 0)  // buffer is full
        return 0;
    client.unsent_data += html_page.substr(0, max_bytes);
    html_page.erase(0, max_bytes);
    table.set_pollout(client.socket);
    if (html_page.empty()) return 1;
    return 0;
}

// TODO: look if vserver/route define a default error page and use it
// instead of generating the standard one
int ErrorRH::setup() {
    struct stat sb;
    std::string err_page;

    try {
        if (request->route->error_pages.empty()) {
            err_page = request->vserver->err_pages.at(error_code);
        } else {
            err_page = request->route->error_pages.at(error_code);
        }
        fd = open(err_page.c_str(), O_RDONLY);
        if (fd < 0) {
            res_type = sending_default;
            generate_error_page();
        } else {
            res_type = sending_file;
        }
    } catch (const std::exception &e) {
        res_type = sending_default;
        generate_error_page();
    }
    // fill-in header
    response.http_version = "HTTP/1.1";
    response.status_code_phrase =
        long_to_str(error_code) + ' ' + reason_phrases[error_code];

    if (fstat(fd, &sb) == -1) {
        response.header_fields["content-length"] =
            long_to_str(html_page.length());
    } else {
        response.header_fields["content-length"] = long_to_str(sb.st_size);
    }
    // TODO: and many other header_fields here.....

    assemble_header_str();
    return (0);
}

// returns 1 if response if complete
// 0 if response not yet complete
// -1 if response was aborted
int ErrorRH::respond() {
    if (state == s_setup) {
        setup();
        state = s_sending_header;
    }
    if (state == s_sending_header) {
        if (send_header() == 1) {
            if (res_type == sending_default)
                state = s_sending_html_str;
            else
                state = s_start_send_file;
        }
    }
    if (state == s_sending_html_str) {
        if (send_html_str() == 1) state = s_done;
    } else if (state == s_start_send_file) {
        table.add_file_fd(fd, request->client);
        state = s_sending_file;
        return 0;
    }
    if (state == s_sending_file) {
        if (table[fd].is_EOF) {
            close(fd);
            table.remove_fd(fd);
            state = s_done;
        }
    }
    if (state == s_done) return (1);
    if (state == s_abort) return (-1);
    return (0);
}

void ErrorRH::abort() { state = s_abort; }

// static function
std::map<int, std::string> ErrorRH::init_map() {
    std::map<int, std::string> map;

    map[404] = "Not Found";
    map[403] = "Forbidden";

    return map;
}

// static variable
std::map<int, std::string> ErrorRH::reason_phrases = init_map();
