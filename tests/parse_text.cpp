#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

int parse_types_file(std::map<std::string, std::string> &map)
{
	std::fstream fs("../conf/mime.types");
	std::string pre_block;
	std::string block;
	std::string line;
	std::string media_type;
	std::string extension;

	// pre-block: expect "types"
	std::getline(fs, pre_block, '{'); 
	std::istringstream pre_block_stream(pre_block);
	pre_block_stream >> line;
	if (line != "types" )
		return (-1);
	
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

int main()
{
	std::map<std::string, std::string> map;

	parse_types_file(map);

	for (std::map<std::string, std::string>::iterator it = map.begin();
		 it != map.end(); it++)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	


}