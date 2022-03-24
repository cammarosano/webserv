#include "includes.hpp"

int parse_mime_types_file(std::map<std::string, std::string> &map)
{
	std::fstream fs("conf/mime.types");
	std::string pre_block;
	std::string block;
	std::string line;
	std::string media_type;
	std::string extension;

    if (!fs.is_open())
    {
        std::cout << "Failed to load conf/mime.types" << std::endl;
        return (-1);
    }
	// pre-block: expect "types"
	std::getline(fs, pre_block, '{'); 
	std::istringstream pre_block_stream(pre_block);
	pre_block_stream >> line;
	if (line != "types" )
    {
        std::cout << "conf/mime.types: bad format" << std::endl;
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

