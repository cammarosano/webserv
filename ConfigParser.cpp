
#include "ConfigParser.hpp"

ConfigParser::ConfigParser(std::string &file_name) : curr_vs(NULL) {
    _f.open(file_name);

    if (!_f.good()) {
        std::cout << "Error: failed to open: " << file_name << std::endl;
        exit(EXIT_FAILURE);
    }
    _parse_config_file();
}

ConfigParser::~ConfigParser() {}

int ConfigParser::_parse_location(std::istringstream &curr_iss) {
    std::string temp;
    std::string line;

    // should be the route
    curr_iss >> temp;
    Route r(temp);
    temp.clear();
    // should be opening brackets
    curr_iss >> temp;
    if (temp != "{") {
        std::cerr << "Error: config file" << std::endl;
        exit(EXIT_FAILURE);
    }
    // can check if there is something after the opening bracket;
    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> temp;
        if (temp == "root") {
            temp.clear();
            iss >> temp;
            size_t c = temp.find(";");
            if (c == std::string::npos) {
                std::cout << "Error: config file" << std::endl;
                exit(EXIT_FAILURE);
            }
            temp = temp.substr(0, c);
            r.root = temp;
            curr_vs->routes.push_back(r);
        }
        // only parsing root, should parse others (cgi, auto_index, ...)
        // TODO: check for syntax errors
        // assume there is no syntax error for now :)
        if (temp == "}") break;
    }
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
            std::cout << "Error config file" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    if (c == std::string::npos) {
        std::cerr << "Error: parse" << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}

int ConfigParser::_parse_port(std::istringstream &curr_iss) {
    std::string port;
    Vserver vs;

    curr_iss >> port;
    size_t c = port.find(";");
    if (c == std::string::npos) {
        std::cerr << "Error: parsing error" << std::endl;
        exit(EXIT_FAILURE);
    }
    port = port.substr(0, c);
    vs.listen = std::make_pair("127.0.0.1", std::stoi(port));
    _config[vs.listen].push_back(vs);
    curr_vs = &(*_config[vs.listen].rbegin());
    return 0;
}

int ConfigParser::_parse_server_block(std::istringstream &prev_iss) {
    // can check for synyax erros using prev_iss
    std::string str;
    std::string line;

    // parse everything in the server block
    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> str;
        if (str == "listen") {
            _parse_port(iss);
        } else if (str == "server_name") {
            _parse_server_names(iss);
        } else if (str == "location") {
            _parse_location(iss);
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
            // give iss to evantualy check for syntax errors
            _parse_server_block(iss);
        }
        block.clear();
    }
    _f.close();
    return 1;
}
