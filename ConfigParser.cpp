
#include "ConfigParser.hpp"

ConfigParser::ConfigParser(std::string &file_name) {
    _f.open(file_name);

    if (!_f.good()) {
        std::cout << "Error: failed to open: " << file_name << std::endl;
        exit(EXIT_FAILURE);
    }
    _parse_config_file();
}

ConfigParser::~ConfigParser() {}

int ConfigParser::_parse_server_names(std::istringstream &curr_iss) {
    std::string serv_name;
    Vserver vs;

    curr_iss >> serv_name;
    while (serv_name.length() != 0) {
        std::cout << "server name: " << serv_name << std::endl;
        serv_name.clear();
        curr_iss >> serv_name;
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

    return 0;
}

int ConfigParser::_parse_server_block(std::istringstream &prev_iss) {
    // can check for synyax erros using iss
    std::string str;
    std::string line;

    // parse everything in the server block
    while (std::getline(_f, line)) {
        std::istringstream iss(line);
        iss >> str;
        if (str == "listen") {
            std::cout << "parsing port" << std::endl;
            _parse_port(iss);
        } else if (str == "server_name") {
            std::cout << "parsing server names" << std::endl;
            // need to know on witch server to push the server names
            _parse_server_names(iss);
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
            std::cout << "server" << std::endl;
            _parse_server_block(iss);
        }
        // give iss to evantualy check for syntax errors
        block.clear();
    }
    _f.close();
    return 1;
}