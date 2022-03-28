#include "includes.hpp"

int open_listening_sockets(FdManager &table,
                            std::map<ip_port, std::list<Vserver> > &config)
{
    std::map<ip_port, std::list<Vserver> >::iterator it;

    for (it = config.begin(); it != config.end(); ++it)
    {
        std::string ip = it->first.first;
        unsigned short port = it->first.second;

        int listening_socket = get_listening_socket(ip, port);
        if (listening_socket == -1)
        {
            clear_resources(table);
            return (-1);
        }
        table.add_listen_socket(listening_socket, it->second);
    }
    return (0);
}

std::string resolve_file_name(int argc, char **argv)
{
    if (argc == 1)
        return ("conf/default.conf");
    return (argv[1]);
}

int setup(FdManager &table, int argc, char **argv)
{
    std::string file_name = resolve_file_name(argc, argv);
    ConfigParser parser(file_name);
    std::map<ip_port, std::list<Vserver> > config = parser.getConfig();

    // mime.types file
    parse_mime_types_file(AReqHandler::content_type);

    if (open_listening_sockets(table, config) == -1)
        return (-1);

    return (0);
}
