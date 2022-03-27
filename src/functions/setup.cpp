#include "includes.hpp"

int open_listening_sockets(FdManager &table, std::map<ip_port, std::list<Vserver> > &config)
{
    typedef std::map<ip_port, std::list<Vserver> >::iterator iterator;

    for (iterator it = config.begin(); it != config.end(); ++it)
    {
        std::string ip = it->first.first;
        unsigned short port = it->first.second;

        int listening_socket = get_listening_socket(ip, port);
        // skipping error check... :/

        table.add_listen_socket(listening_socket, it->second);
    }
    return (0);
}

int setup(FdManager &table)
{
    ConfigParser parser;
    std::map<ip_port, std::list<Vserver> > config;
    try
    {
      config = parser.parse("./conf/default.conf"); 
    }
    catch (const ConfigParser::ConfigParserException& e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    // content-types file
    parse_mime_types_file(AReqHandler::content_type);

    open_listening_sockets(table, config);

    return (0);
}
