#if !defined(__CONFIG_PARSER_H__)
#define __CONFIG_PARSER_H__

#include <fstream>
#include <iostream>
#include <map>

#include "includes.hpp"

class ConfigParser {
   private:
    std::map<ip_port, std::list<Vserver> > _config;
    std::fstream _f;

   public:
    ConfigParser(std::string &file_name);
    ~ConfigParser();

    const std::map<ip_port, std::list<Vserver> > &getConfig() const {
        return _config;
    }

   private:
    int _parse_config_file();
    int _parse_server_block(std::istringstream &prev_iss);
    int _parse_port(std::istringstream &curr_iss);
    int _parse_server_names(std::istringstream &curr_iss);
};

#endif  // __CONFIG_PARSER_H__
