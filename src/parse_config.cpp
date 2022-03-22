#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "ConfigParser.hpp"
#include "macros.h"

int main() {
    std::string file_name = "default.conf";
    ConfigParser parser(file_name);
    std::map<ip_port, std::list<Vserver> > config = parser.getConfig();
    std::map<ip_port, std::list<Vserver> >::iterator it = config.begin();
    for (; it != config.end(); ++it) {
        std::list<Vserver>::iterator vsit = it->second.begin();
        for (; vsit != it->second.end(); ++vsit) {
            std::cout << "----------------------------------\n";
            std::list<Route>::iterator rou_it = (*vsit).routes.begin();
            for (; rou_it != (*vsit).routes.end(); rou_it++) {
                std::cout << "Auto index: " << rou_it->auto_index << std::endl;
                std::list<std::string> am = rou_it->accepted_methods;
                for (std::list<std::string>::iterator am_it = am.begin();
                     am_it != am.end(); ++am_it) {
                    std::cout << "allowed methods " << *am_it << ", ";
                }
                std::cout << '\n';
                std::cout << GREEN << "prefix: " << rou_it->prefix << RESET
                          << std::endl;
                std::cout << GREEN << "default index: " << rou_it->default_index
                          << RESET << std::endl;
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
