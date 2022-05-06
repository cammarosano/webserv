#include "BodyDecoder.hpp"

BodyDecoder::BodyDecoder(HttpRequest &request)
	: raw_data(request.client.received_data),
	  decoded_data(request.client.decoded_body), length_decoded(0)
{
	type = resolve_type(request);
	if (type == chunked)
	{
		bytes_left_last_chunk = 0;
		removeCRLF = false;
		done = false;
	}
	else if (type == content_length)
	{
		content_len = std::strtol(
			request.header_fields["content-length"].c_str(), NULL, 10);
		bytes_left = content_len;
	}
}

BodyDecoder::~BodyDecoder()
{
}

BodyDecoder::e_type BodyDecoder::resolve_type(HttpRequest &request)
{
	std::map<std::string, std::string>::iterator it;

	it = request.header_fields.find("transfer-encoding");
	if (it != request.header_fields.end())
	{
		if (it->second.find("chunked") != std::string::npos)
			return (chunked);
		return (other); // transfer-enconding non-chunked -> other
	}
	it = request.header_fields.find("content-length");
	if (it != request.header_fields.end())
		return (content_length);
	return (other);
}

// returns 1 if complete, 0 if incomplete, -1 if error
int BodyDecoder::decode_body()
{
	if (type == content_length)
		return (decode_known_len());
	if (type == chunked)
	{
		if (done)
			return (1);
		return (decode_chunked());
	}
	return (-1);
}

// returns the minimum of 3 ints
long min_of_3(long a, long b, long c)
{
	long min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return (min);
}

// Transfer up to n bytes from raw_data to decoded_data
// Returns the amount of bytes transfered, which depends on
// destination's free space and sources' available bytes
// Updates length_decoded
int BodyDecoder::transfer_n_bytes(long n)
{
	int free_space = BUFFER_SIZE - decoded_data.size();
	int bytes_available = raw_data.size();
	int transf_bytes = min_of_3(n, free_space, bytes_available);

	if (transf_bytes <= 0)
		return (0);
	decoded_data.append(raw_data, 0, transf_bytes);
	raw_data.erase(0, transf_bytes);
	length_decoded += transf_bytes;
	return (transf_bytes);
}

int BodyDecoder::decode_known_len()
{
	bytes_left -= transfer_n_bytes(bytes_left);
	if (!bytes_left)
		return (1); // complete
	return (0);
}

// remove eventual trailer fields and the last CRLF
// returns 0 if not enought data to finish it
int BodyDecoder::finish()
{
	size_t pos;

	pos = raw_data.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (0);
	raw_data.erase(0, pos + 4);
	done = true;
	return (1);
}

// format: 1*HEXDIG [';' whatever] CRLF
bool validade_chunk_size_line(std::string &line, size_t pos_CRLF)
{
	if (!isxdigit(line[0]))
		return (false);
	size_t i = 1;
	while (isxdigit(line[i]))
		++i;
	if (i == pos_CRLF || isspace(line[i]) || line[i] == ';')
		return (true);
	return (false);
}

int BodyDecoder::decode_chunked()
{
	size_t pos;
	long chunk_size;

	if (bytes_left_last_chunk)
		bytes_left_last_chunk -= transfer_n_bytes(bytes_left_last_chunk);
	while (!bytes_left_last_chunk) // new chunk
	{
		if (raw_data.size() < 2)
			return (0); // come back after another IO round
		if (removeCRLF) // remove CLRF end of last chunk
		{
			if (raw_data.substr(0, 2) != "\r\n") // validation
				return (-1);
			raw_data.erase(0, 2);
			removeCRLF = false;
		}
		// parse chunk-size
		pos = raw_data.find("\r\n");
		if (pos == std::string::npos) // CRLF not found
			return (0);				  // come back after another IO round
		if (!validade_chunk_size_line(raw_data, pos))
			return (-1);
		chunk_size = std::strtol(raw_data.c_str(), NULL, 16);
		if (chunk_size == 0)
			return (finish());
		raw_data.erase(0, pos + 2); // discard chunk-size line
		bytes_left_last_chunk = chunk_size - transfer_n_bytes(chunk_size);
		removeCRLF = true;
	}
	return (0);
}

size_t BodyDecoder::getLengthDecoded() const
{
	return (length_decoded);
}
