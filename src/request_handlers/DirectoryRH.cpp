#include "DirectoryRH.hpp"

DirectoryRH::DirectoryRH(HttpRequest *request, FdManager &table, const std::string &path)
    : AReqHandler(request, table), ressource_path(path)
{
    _setup();
    state = s_sending_header;
}

DirectoryRH::~DirectoryRH()
{
}

std::list<std::pair<std::string, unsigned char> > DirectoryRH::_get_files_name()
{
    struct dirent *dir;
    std::list<std::pair<std::string, unsigned char> > files;
    DIR *d = opendir(ressource_path.c_str());

    if (!d)
        throw std::exception();
    while ((dir = readdir(d)) != NULL)
    {
        if (dir->d_type == DT_DIR)
            files.push_front(std::make_pair(dir->d_name, dir->d_type));
        else
            files.push_back(std::make_pair(dir->d_name, dir->d_type));
    }
    closedir(d);
    return files;
}

void DirectoryRH::_generate_autoindex_page()
{
    std::ostringstream res;
    std::list<std::pair<std::string, unsigned char> > files = _get_files_name();

    res << "<!DOCTYPE html>\n";
    res << "<html>\n";
    res << "<head>\n";
    res << "<title>" << ressource_path << "</title>\n";
    res << "</head>\n";
    res << "<body>\n";
    std::list<std::pair<std::string, unsigned char> >::iterator f_it = files.begin();
    
    while (f_it != files.end())
    {
        std::string param = request->target;
        std::string::iterator it = --(param.end());
        if (*it == '/')
            param.erase(it);
        if (param == "/")
            param.clear();
        res << "<a href=";
        res << "\"" << param << "/" << f_it->first;
        if (f_it->second == DT_DIR)
            res << "/";
        res << "\" >";
        res << f_it->first;
        if (f_it->second == DT_DIR)
            res << "/";
        res << "</a> <br>";
        ++f_it;
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
