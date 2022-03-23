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

int parse_types_file(std::map<std::string, std::string> &map)
{
	std::fstream fs("conf/mime.types");
	std::string pre_block;
	std::string block;
	std::string line;
	std::string media_type;
	std::string extension;

    if (!fs.is_open())
    {
        std::cout << "Error: mime types file not found" << std::endl;
        return (-1);
    }
	// pre-block: expect "types"
	std::getline(fs, pre_block, '{'); 
	std::istringstream pre_block_stream(pre_block);
	pre_block_stream >> line;
	if (line != "types" )
    {
        std::cout << "Error: mime types file bad format" << std::endl;
		return (-1);
    }
	
	// block 
	std::getline(fs, block, '}');
	std::istringstream block_stream(block);

	while (!block_stream.eof())
	{
		// line
		std::getline(block_stream, line, ';');
		std::istringstream ls(line);
		// media-type
		ls >> media_type;
		while (!ls.eof())
		{
			ls >> extension;
			map[extension] = media_type;
		}
		
	}
	return (0);
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

    // content-types file
    if (parse_types_file(AReqHandler::content_type) == -1)
        return (-1);
    std::cout << "Types files parsing OK. "
        << AReqHandler::content_type.size() << " extensions mapped."
        << std::endl;

    open_listening_sockets(table, config);

    return (0);
}
