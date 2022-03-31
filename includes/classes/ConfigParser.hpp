#if !defined(__CONFIG_PARSER_H__)
#define __CONFIG_PARSER_H__

#include "config.hpp"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <map>
#include <cstdlib>
#include <exception>

class ConfigParser {
  private:
    std::map<ip_port, std::list<Vserver> > _config;
    std::fstream _f;
    Vserver *curr_vs;

  public:
    ConfigParser();
    ~ConfigParser();

    const std::map<ip_port, std::list<Vserver> > &parse(const char *file_name);

    class ConfigParserException : std::exception {
      private:
        const char *message;

      public:
        const char *what() const throw() { return message; }
        ConfigParserException(const char *m) : message(m) {}
    };

  private:
    int _parse_config_file();
    int _parse_server_block();
    int _parse_port(std::istringstream &iss);
    int _parse_server_names(std::istringstream &iss);
    int _parse_location(std::istringstream &iss);
    void _parse_error_page(std::istringstream &iss);
    void _parse_redirection(std::istringstream &iss);

    void _parse_root(std::istringstream &iss, Route &r);
    void _parse_auto_index(std::istringstream &iss, Route &r);
    void _parse_default_index(std::istringstream &iss, Route &r);
    void _parse_cgi_interpreter(std::istringstream &iss, Route &r);
    void _parse_cgi_extension(std::istringstream &iss, Route &r);
    void _parse_allowed_methods(std::istringstream &iss, Route &r);
    void _parse_route_error_page(std::istringstream &iss, Route &r);
    void _parse_route_redirection(std::istringstream &iss, Route &r);
    void _parse_route_upload(std::istringstream &iss, Route &r);
    void _parse_route_max_body_size(std::istringstream &iss, Route &r);
};

#endif // __CONFIG_PARSER_H__
