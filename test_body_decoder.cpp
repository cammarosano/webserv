#include "BodyDecoder.hpp"
#include "HttpRequest.hpp"

// CHANGE BUFFER SIZE WHEN TESTING!

int main()
{
	Vserver vserver;
	std::list<Vserver> lst;
	lst.push_back(vserver);
{
	std::cout << "Testing Content-Length" << std::endl;
	Client client(0, lst);
	std::string header_str(
		"POST / HTTP/1.1\r\n"
		"Content-Length: 30\r\n"
		"\r\n"
	);
	client.received_data = "01234567890";

	HttpRequest request(client, header_str);
	BodyDecoder bd(request);
	int ret = bd.decode_body();
	while (ret == 0)
	{
		std::cout << "decoded_body buffer:" << client.decoded_body << std::endl;
		std::cout << "length transfered: " << bd.getLengthDecoded() << std::endl;

		// IO round
		client.decoded_body.erase(0, 4);
		client.received_data.append("abc");

		ret = bd.decode_body();
	}
	std::cout << "Finished. length transfered: " << bd.getLengthDecoded() << std::endl;
	std::cout << "decoded_body buffer:" << client.decoded_body << std::endl;

}
{
	std::cout << "Testing Transfer-Encoding" << std::endl;
	Client client(0, lst);
	std::string header_str(
		"POST / HTTP/1.1\r\n"
		"Transfer-Encoding: chunked\r\n"
		"\r\n"
	);
	std::string body(
		"4\r\n"
		"1234\r\n"
		"8\r\n"
		"56789abc\r\n"
		"3\r\n"
		"def\r\n"
		"E\r\n"
		"12345678901234\r\n"
		"0\r\n"
		"\r\n"
	);
	client.received_data = body.substr(0,10);
	body.erase(0, 10);
	HttpRequest request(client, header_str);
	BodyDecoder bd(request);
	int ret = bd.decode_body();
	while (ret == 0)
	{
		std::cout << "decoded_body buffer:" << client.decoded_body << std::endl;
		std::cout << "length transfered: " << bd.getLengthDecoded() << std::endl;

		// IO round

		client.unsent_data.append(client.decoded_body.substr(0,4));
		client.decoded_body.erase(0, 4);
		client.received_data.append(body.substr(0,4));
		if (!body.empty())
			body.erase(0, 4);

		ret = bd.decode_body();
		
	}
	std::cout << "Finished. length transfered: " << bd.getLengthDecoded() << std::endl;
	std::cout << "unsent_data buffer:" << client.unsent_data << std::endl;

}

}