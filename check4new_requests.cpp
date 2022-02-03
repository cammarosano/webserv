#include "includes.hpp"
#include "FdManager.hpp"
#include "StaticRH.hpp"

// this parser must be improved: check errors...
int parse_header(std::string &header_str, HttpRequest &request)
{
	std::istringstream	stream(header_str);

	stream >> request.method;
	stream >> request.target;
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
		std::string field_value = line.substr(delimiter_pos + 1);
		remove_trailing_spaces(field_value);
		request.header_fields[field_name] = field_value;
	}
	return (0);
}

// extracts data from the client's received_data buffer into a HttpRequest object
// return NULL if buffer does not contain a complete request header
HttpRequest * new_HttpRequest(Client &client)
{
	// look for end-of-header delimiter: 2CRLF
	size_t pos = client.received_data.find("\r\n\r\n");

	if (pos == std::string::npos) // not found: header is incomplete
		return (NULL);

	// header is complete: consume data
	std::string header_str = client.received_data.substr(0, pos);
	client.received_data.erase(0, pos + 4);

	// debug
	std::cout << "---------\nThe following request header was received:\n"
				<< header_str << "\n" << std::endl;
	
	// create HttpRequest object
	HttpRequest *request = new HttpRequest(client);

	// parse header into a new Request
	parse_header(header_str, *request);

	return (request);
}

ARequestHandler *init_response(HttpRequest &request, FdManager &table)
{
	// resolve vserver and route
	// TODO: transform these into methods of HttpRequest
	Vserver &vserver = resolve_vserver(request);
	request.vserver = &vserver;
	request.route = resolve_route(vserver, request.target);

	// resolve type
	e_rhtype type;

	// TODO. for now, type is t_static
	type = t_static;

	// if (type == t_static)

	return (new StaticRH(&request, table));
}

int check4new_requests(FdManager &table,
						std::list<ARequestHandler*> &req_handlers_lst)
{
	// iterate over clients in recv_header state
	for (int fd = 3; fd < table.len(); ++fd)
	{
		if (table[fd].type != fd_client_socket)
			continue;

		Client &client = *table[fd].client;

		if (client.state != recv_header || client.received_data.empty())
			continue;
		HttpRequest *request = new_HttpRequest(client);
		if (!request)
			continue;
		ARequestHandler *req_handler = init_response(*request, table);
		req_handlers_lst.push_back(req_handler);
		client.state = handling_response;
		client.ongoing_response = req_handler;
	}
	return (0);
}
