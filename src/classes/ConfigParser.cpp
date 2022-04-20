#include "ConfigParser.hpp"

ConfigParser::ConfigParser() : curr_vs(NULL) {}

ConfigParser::~ConfigParser() {}

const std::map<ip_port, std::list<Vserver> > &ConfigParser::parse(
    const char *file_name) {
    _f.open(file_name);
    if (!_f.good()) {
        throw ConfigParser::ConfigParserException(
            "Error: failed to open the file");
    }
    _parse_config_file();
    return _config;
}

void ConfigParser::_parse_root(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: server root");
    }
    parsed = parsed.substr(0, c);
    r.root = parsed;
}

void ConfigParser::_parse_auto_index(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: server auto index");
    }
    parsed = parsed.substr(0, c);
    if (parsed == "on") {
        r.auto_index = true;
    } else if (parsed == "off") {
        r.auto_index = false;
    } else {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: Invalide value for autoindex, it should be "
            "(on | off)");
    }
}

void ConfigParser::_parse_default_index(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: server default index");
    }
    r.default_index = parsed.substr(0, c);
}

void ConfigParser::_parse_cgi_interpreter(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route cgi interpreter");
    }
    parsed = parsed.substr(0, c);
    r.cgi_interpreter = parsed;
}

void ConfigParser::_parse_cgi_extension(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route cgi extension");
    }
    parsed = parsed.substr(0, c);
    r.cgi_extension = parsed;
}

void ConfigParser::_parse_allowed_methods(std::istringstream &iss, Route &r) {
    std::string parsed;
    size_t c;

    iss >> parsed;
    while (!parsed.empty()) {
        c = parsed.find(";");
        if (c != std::string::npos) {
            parsed = parsed.substr(0, c);
        }
        r.accepted_methods.push_back(parsed);
        parsed.clear();
        iss >> parsed;
    }
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route allowed methods");
    }
}

void ConfigParser::_parse_route_error_page(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    if (!str_is_number(parsed)) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: error status code should be a number");
    }
    int err_code = std::strtol(parsed.c_str(), NULL, 10);
    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        std::cerr << "Error: Config file" << std::endl;
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route error page");
    }
    r.error_pages[err_code] = parsed.substr(0, c);
}

void ConfigParser::_parse_route_redirection(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    if (!str_is_number(parsed)) {
        std::cerr << "Error: Config file" << std::endl;
        throw ConfigParser::ConfigParserException(
            "Error: Config file: redirection status code should be a number");
    }
    int status_code = std::strtol(parsed.c_str(), NULL, 10);
    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route redirection");
    }
    r.redirect.location = parsed.substr(0, c);
    r.redirect.status_code = status_code;
    r.redirected = true;
}

void ConfigParser::_parse_route_upload(std::istringstream &iss, Route &r) {
    std::string parsed;

    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route upload accepted");
    }
    r.upload_accepted = true;
    r.upload_dir = parsed.substr(0, c);
}

void ConfigParser::_parse_route_max_body_size(std::istringstream &iss,
                                              Route &r) {
    std::string parsed;
    bool mega = false;

    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: route max body size");
    }
    parsed = parsed.substr(0, c);
    if (*(parsed.rbegin()) == 'M') {
        mega = true;
        parsed = parsed.substr(0, parsed.length() - 1);
    }
    if (!str_is_number(parsed)) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: max body size should be a number");
    }
    r.body_size_limit = std::atoi(parsed.c_str());
    if (mega) r.body_size_limit *= 1000000;
}

// TODO: cut this function in pieces
int ConfigParser::_parse_location(std::istringstream &curr_iss) {
    std::string temp;
    std::string line;

    curr_iss >> temp;
    Route r(temp);
    temp.clear();
    curr_iss >> temp;
    if (temp != "{") {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: syntax error");
    }
    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> temp;
        if (temp == "root") {
            _parse_root(iss, r);
        } else if (temp == "autoindex") {
            _parse_auto_index(iss, r);
        } else if (temp == "index") {
            _parse_default_index(iss, r);
        } else if (temp == "cgi_interpreter") {
            _parse_cgi_interpreter(iss, r);
        } else if (temp == "cgi_extension") {
            _parse_cgi_extension(iss, r);
        } else if (temp == "allowed_methods") {
            _parse_allowed_methods(iss, r);
        } else if (temp == "error_page") {
            _parse_route_error_page(iss, r);
        } else if (temp == "return") {
            _parse_route_redirection(iss, r);
        } else if (temp == "upload_dir") {
            _parse_route_upload(iss, r);
        } else if (temp == "max_body_size") {
            _parse_route_max_body_size(iss, r);
        }
        if (temp == "}") break;
    }
    curr_vs->routes.push_back(r);
    return 0;
}

int ConfigParser::_parse_server_names(std::istringstream &curr_iss) {
    std::string serv_name;
    Vserver vs;
    size_t c;

    curr_iss >> serv_name;
    while (serv_name.length() != 0) {
        c = serv_name.find(";");
        if (c != std::string::npos) serv_name = serv_name.substr(0, c);

        curr_vs->server_names.push_back(serv_name);
        serv_name.clear();
        curr_iss >> serv_name;

        // Check if there is something after the semi colon
        if (c != std::string::npos && serv_name.length() != 0) {
            throw ConfigParser::ConfigParserException(
                "Error: Config file: server names");
        }
    }
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config: syntax error");
    }
    return 0;
}

int ConfigParser::_parse_port(std::istringstream &curr_iss) {
    std::string port;
    Vserver vs;

    curr_iss >> port;
    size_t c = port.find(";");
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config: syntax error");
    }
    port = port.substr(0, c);
    vs.listen = std::make_pair("127.0.0.1", std::atoi(port.c_str()));
    _config[vs.listen].push_back(vs);
    curr_vs = &(*_config[vs.listen].rbegin());
    return 0;
}

void ConfigParser::_parse_error_page(std::istringstream &iss) {
    std::string parsed;

    iss >> parsed;
    if (!str_is_number(parsed)) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: server error status code should be a number");
    }
    int err_code = std::strtol(parsed.c_str(), NULL, 10);
    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config: syntax error");
    }
    curr_vs->err_pages[err_code] = parsed.substr(0, c);
}

void ConfigParser::_parse_redirection(std::istringstream &iss) {
    std::string parsed;

    iss >> parsed;
    if (!str_is_number(parsed)) {
        throw ConfigParser::ConfigParserException(
            "Error: Config file: server redirection status code should be a "
            "number");
    }
    int status_code = std::strtol(parsed.c_str(), NULL, 10);
    iss >> parsed;
    size_t c = parsed.find(';');
    if (c == std::string::npos) {
        throw ConfigParser::ConfigParserException(
            "Error: Config: syntax error");
    }
    curr_vs->redirected = true;
    curr_vs->redirect.location = parsed.substr(0, c);
    curr_vs->redirect.status_code = status_code;
}

int ConfigParser::_parse_server_block() {
    std::string str;
    std::string line;

    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> str;
        if (*(str.begin()) == '#') continue;

        if (str == "listen") {
            _parse_port(iss);
        } else if (str == "server_name") {
            _parse_server_names(iss);
        } else if (str == "location") {
            _parse_location(iss);
        } else if (str == "error_page") {
            _parse_error_page(iss);
        } else if (str == "return") {
            _parse_redirection(iss);
        }
        str.clear();
    }
    return 1;
}

int ConfigParser::_parse_config_file() {
    std::string block;
    std::string line;

    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> block;
        if (block == "server") {
            _parse_server_block();
        }
        block.clear();
    }
    _f.close();
    return 1;
}
