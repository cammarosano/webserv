#include "ConfigParser.hpp"
#include "FdManager.hpp"
#include "includes.hpp"

// I made localhost-example and localhost-example2 map to 127.0.0.1 on my
// machine

// hardcoded configuration for testing purposes
void get_test_config(std::map<ip_port, std::list<Vserver> > &config) {
    {
        Vserver vs;

        vs.listen = ip_port("127.0.0.1", 3000);
        {
            Route r("/");

            r.root = "web_root";
            vs.routes.push_back(r);
        }
        config[vs.listen].push_back(vs);
    }

    {
        Vserver vs;

        vs.listen = ip_port("127.0.0.1", 3000);
        vs.server_names.push_back("localhost");
        vs.server_names.push_back("localhost-example");
        {
            Route r("/");

            r.root = "web_root/localhost";
            vs.routes.push_back(r);
        }
        config[vs.listen].push_back(vs);
    }

    {
        Vserver vs;

        vs.listen = ip_port("127.0.0.1", 3001);
        vs.server_names.push_back("localhost");
        {
            Route r("/");

            r.root = ".";
            vs.routes.push_back(r);
        }
        {
            Route r("/images/");

            r.root = "web_root/imgs";
            vs.routes.push_back(r);
        }
        config[vs.listen].push_back(vs);
    }
}

int parse_config_file(std::map<ip_port, std::list<Vserver> > &config) {
    // for now, get hardcoded config for testing
    get_test_config(config);
    return (0);
}

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

    // parse_config_file(config);

    open_listening_sockets(table, config);

    return (0);
}