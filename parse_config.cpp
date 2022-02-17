#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ConfigParser.hpp"

#define OPEN "{"
#define CLOSE "}"

/* parse everything in the server block */
int parse_server_block(std::fstream &f, std::istringstream &iss, Vserver &vs) {
    std::string block;
    std::string next;
    std::string line;

    while (std::getline(f, line)) {
        std::istringstream temp_iss(line);
        temp_iss >> block;
        if (block == "listen") {
            std::string port;
            temp_iss >> port;
            size_t c = port.find(";");
            if (c == std::string::npos) {
                // parsing error;
                std::cout << "Error: parsing error" << std::endl;
                exit(EXIT_FAILURE);
            }
            port = port.substr(0, c);
            std::cout << "listening on port: " << port << std::endl;
            vs.listen = std::make_pair("127.0.0.1", std::stoi(port));
        } else if (block == "server_name") {
            std::string serv_name;
            temp_iss >> serv_name;
            while (serv_name.length() != 0) {
                vs.server_names.push_back(serv_name);
                serv_name.clear();
                temp_iss >> serv_name;
            }
        }
        block.clear();
    }
    return 1;
}

int parse_config(std::string &file_name, Vserver &vs) {
    std::fstream f(file_name);
    std::string line;
    std::string block;
    std::string next;
    if (!f.good()) return -1;

    /* parse all servers in the config file */
    while (std::getline(f, line)) {
        std::istringstream temp_iss(line);
        temp_iss >> block >> next;
        if (block == "server" && next == OPEN) {
            std::cout << "Parse server block" << std::endl;
            parse_server_block(f, temp_iss, vs);
        }
        block.clear();
        next.clear();
    }
    f.close();
    return 1;
}

int main() {
    std::string file_name = "default.conf";
    //     Vserver vs;
    //     parse_config(file_name, vs);
    //     std::list<std::string>::iterator it = vs.server_names.begin();
    //     for (; it != vs.server_names.end(); ++it) {
    //         std::cout << *it << std::endl;
    //     }
    //     std::cout << "Listening on port: " << vs.listen.second << std::endl;
    ConfigParser parser(file_name);
    std::map<ip_port, std::list<Vserver> > config = parser.getConfig();
    std::map<ip_port, std::list<Vserver> >::iterator it = config.begin();
    Vserver vs = *((it->second).begin());
    std::list<std::string>::iterator cit = vs.server_names.begin();
    std::cout << config.begin()->second.size() << std::endl;
    for (; cit != vs.server_names.end(); ++cit) {
        std::cout << "start" << std::endl;
        std::cout << (*cit).c_str() << std::endl;
    }
    return 0;
}
