#include "includes.hpp"
#include "FdManager.hpp"

// this parser must be improved: check errors...
int parse_header(std::string &header_str, HttpRequest &request)
{
	std::istringstream	stream(header_str);

	stream >> request.method;
	stream >> request.request_target;
	stream >> request.http_version;
	
	while (!stream.eof())
	{
		std::string line;
		getline(stream, line);
		size_t delimiter_pos = line.find(':');
		if (delimiter_pos == std::string::npos) 
			continue;
		std::string field_name = line.substr(0, delimiter_pos);
		str_tolower(field_name);
		request.header_fields[field_name] = line.substr(delimiter_pos + 1);
	}
	return (0);
}

// 0: header is not yet complete
// 1: a request was added to the queue
int get_request_header(Client &client, std::queue<HttpRequest> &requests)
{
	// look for end-of-header delimiter: 2CRLF
	size_t pos = client.received_data.find("\r\n\r\n");

	if (pos == std::string::npos) // not found: header is incomplete
		return (0);

	// header is complete: consume data
	std::string header_str = client.received_data.substr(0, pos);
	client.received_data.erase(0, pos + 4);

	// debug
	std::cout << "The following request header was received:\n"
				<< header_str << "\n" << std::endl;

	// parse header into a new Request
	HttpRequest request(client);
	parse_header(header_str, request);

	// add request to queue
	requests.push(request);

	return (1);
}

// transforms incoming data into HttpRequest objects
int process_incoming_data(FdManager &table, std::queue<HttpRequest> &requests)
{
	// for each Client, containing unprocessed data, in get_header state
	for (int fd = 3; fd < table.len(); fd++)
	{
		if (table[fd].type != fd_client_socket)
			continue;

		Client &client = *table[fd].client;

		if (client.received_data.empty())
			continue;
		if (client.recv_state == get_header)
			get_request_header(client, requests);

		// todo: recv_state = get_body	??	
	}
	return (0);
}
