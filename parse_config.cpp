#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ConfigParser.hpp"

int main() {
    std::string file_name = "default.conf";
    ConfigParser parser(file_name);
    std::map<ip_port, std::list<Vserver> > config = parser.getConfig();
    std::map<ip_port, std::list<Vserver> >::iterator it = config.begin();
    std::cout << "config size: " << config.size() << std::endl;
    for (; it != config.end(); ++it) {
        std::list<Vserver>::iterator vsit = it->second.begin();
        for (; vsit != it->second.end(); ++vsit) {
            std::list<Route>::iterator rou_it = (*vsit).routes.begin();
            for (; rou_it != (*vsit).routes.end(); rou_it++) {
                std::cout << "Auto index: " << rou_it->auto_index << std::endl;
            }
            std::list<std::string>::iterator snit =
                (*vsit).server_names.begin();
            std::cout << "<" << (*vsit).listen.first << " "
                      << (*vsit).listen.second << "> ";
            for (; snit != (*vsit).server_names.end(); snit++) {
                std::cout << "Server name: " << *snit << " ";
            }
            std::cout << "\n";
        }
    }
    return 0;
}
