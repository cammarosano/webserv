#include "DirectoryRH.hpp"

DirectoryRH::DirectoryRH(HttpRequest *request, FdManager &table, const std::string &path)
    : AReqHandler(request, table), ressource_path(path)
{
    state = s_setup;
}

DirectoryRH::~DirectoryRH()
{
}

void DirectoryRH::_generate_autoindex_page()
{
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
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            std::string param = request->target;
            std::string::iterator it = --(param.end());
            if (*it == '/')
                param.erase(it);
            if (param == "/")
                param.clear();
            res << "<a href=";
            res << "\"" << param << "/" << dir->d_name;
            if (dir->d_type == DT_DIR)
                res << "/";
            res << "\" >";
            res << dir->d_name;
            if (dir->d_type == DT_DIR)
                res << "/";
            res << "</a> <br>";
        }
        closedir(d);
    }
    res << "</body>\n";
    res << "</html>";
    html_page = res.str();
}

int DirectoryRH::_setup()
{
    _generate_autoindex_page();

    response.status_code_phrase = "200 OK";
    response.header_fields["content-length"] = long_to_str(html_page.length());
    response.header_fields["content-type"] = "text/html";
    response.assemble_header_str();
    return 0;
}

int DirectoryRH::respond()
{
    switch (state)
    {
    case s_setup:
        _setup();
        state = s_sending_header;
    case s_sending_header:
        if (send_str(response.header_str) == 0)
            return 0;
        state = s_sending_html_str;
    case s_sending_html_str:
        if (send_str(html_page) == 0)
            return 0;
        state = s_done;
    default:
        return 1;
    }
    return 0;
}
