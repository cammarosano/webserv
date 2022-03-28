#include "ErrorRH.hpp"

ErrorRH::ErrorRH(HttpRequest *request, FdManager &table, int error_code)
    : AReqHandler(request, table), error_code(error_code) {
    state = s_setup;
}

ErrorRH::~ErrorRH() {
    if (res_type == sending_file)
    {
        close(fd);
        table.remove_fd(fd);
    }
}

// true if found, and puts it in file_name
bool ErrorRH::look_up_err_page(std::map<int, std::string> &error_pages,
                                std::string &file_name)
{
    std::map<int, std::string>::iterator it = error_pages.find(error_code);

    if (it == error_pages.end()) // not found
        return (false);
    file_name = it->second;
    return (true);
}

// returns true if there's a custom page and fills in err_page
// false otherwise
bool ErrorRH::custom_error_page(std::string &err_page)
{
    // check if route has custom page
    if (request->route
        && look_up_err_page(request->route->error_pages, err_page))
        return (true);
    // check if vserver has custom page
    else if (look_up_err_page(request->vserver->err_pages, err_page))
        return (true);
    return (false);
}

// resolves response type (default or custom page)
// opens fd || generates default page
// assembles header_str
int ErrorRH::setup() {
    struct stat sb;
    std::string err_page;
    
    // resolve res_type
    res_type = sending_default;
    if (custom_error_page(err_page))
    {
        fd = open(err_page.c_str(), O_RDONLY);
        if (fd != -1 && fstat(fd, &sb) == 0) // if no error
            res_type = sending_file;
    }

    // generate default-page
    if (res_type == sending_default)
        html_page = generate_error_page(error_code);

    // fill-in header
    response.status_code_phrase =
        long_to_str(error_code) + ' ' + reason_phrases[error_code];
    if (res_type == sending_default)
    {
        response.header_fields["content-length"] =
            long_to_str(html_page.size());
        response.header_fields["content-type"] = "text/html";
    }
    else 
    {
        response.header_fields["content-length"] = long_to_str(sb.st_size);
        response.header_fields["content-type"] = get_mime_type(err_page);
    }
    
    // TODO: and many other header_fields here.....
    response.assemble_header_str();   
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
        if (send_str(response.header_str) == 1) {
            if (res_type == sending_default)
                state = s_sending_html_str;
            else
                state = s_start_send_file;
        }
    }
    if (state == s_sending_html_str) {
        if (send_str(html_page) == 1) state = s_done;
    } else if (state == s_start_send_file) {
        table.add_fd_read(fd, request->client);
        state = s_sending_file;
        return 0;
    }
    if (state == s_sending_file) {
        if (table[fd].is_EOF) {
            state = s_done;
        }
    }
    if (state == s_done) return (1);
    return (0);
}

int ErrorRH::time_out_code()
{
    return (0); // means: no error response to inform this time, as this already
                // is an error response
}

// static function
std::map<int, std::string> ErrorRH::init_map() {
    std::map<int, std::string> map;

    map[400] = "Bad Request";
    map[403] = "Forbidden";
    map[404] = "Not Found";
    map[405] = "Method Not Allowed";
    map[408] = "Request Timeout";
    map[413] = "Payload Too Large";
    map[431] = "Request Header Fields Too Large";

    map[500] = "Internal Server Error";
    map[501] = "Not Implemented";
    map[502] = "Bad Gateway";
    map[504] = "Gateway Timeout";

    return map;
}

// static variable
std::map<int, std::string> ErrorRH::reason_phrases = init_map();

// static function
std::string ErrorRH::generate_error_page(int error_code)
{
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

    return (oss.str());
}
