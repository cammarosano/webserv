#include "includes.hpp"

int open_listening_sockets(FdManager &table,
                           std::map<ip_port, std::list<Vserver> > &config) {
    typedef std::map<ip_port, std::list<Vserver> >::iterator iterator;

    for (iterator it = config.begin(); it != config.end(); ++it) {
        std::string ip = it->first.first;
        unsigned short port = it->first.second;

        int listening_socket = get_listening_socket(ip, port);
        // skipping error check... :/

        table.add_listen_socket(listening_socket, it->second);
    }
    return (0);
}

int setup(FdManager &table) {
    std::string file_name = "conf/default.conf";
    ConfigParser parser(file_name);
    std::map<ip_port, std::list<Vserver> > config = parser.getConfig();

    // content-types file
    parse_mime_types_file(AReqHandler::content_type);

    open_listening_sockets(table, config);

    return (0);
}
