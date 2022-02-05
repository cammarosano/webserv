#ifndef ERRORRH_HPP
# define ERRORRH_HPP

# include "includes.hpp"
# include "StaticRH.hpp"

class ErrorRH: public StaticRH // TODO
{
private:
	int fd_error_page;
	int setup();

public:
	ErrorRH(HttpRequest *request, FdManager &table);
	~ErrorRH();

	virtual int respond();
	virtual void abort();

};



// // TODO: check if vserver does not have a different default_403 page
// void StaticRH::setup_403_response()
// {
// 	struct stat sb;

// 	fd_file = open(DEFAULT_403_PAGE, O_RDONLY);
// 	if (fd_file == -1) // TODO: handle this
// 	{
// 		perror("open() default 403 page");
// 	}
// 	if (stat(DEFAULT_403_PAGE, &sb) == -1) // TODO: handle this
// 	{
// 		perror("stat() default 403 page");
// 	}
// 	response.http_version = "HTTP/1.1";
// 	response.status_code_phrase = "403 Forbidden";
// 	response.header_fields["content-length"] = long_to_str(sb.st_size);
// 	assemble_header_str();
// }

// // ! repeated code ! TODO: refactor this...
// void StaticRH::setup_404_response()
// {
// 	struct stat sb;

// 	fd_file = open(DEFAULT_404_PAGE, O_RDONLY);
// 	if (fd_file == -1) // TODO: handle this
// 	{
// 		perror("open() default 404 page");
// 	}
// 	if (stat(DEFAULT_404_PAGE, &sb) == -1) // TODO: handle this
// 	{
// 		perror("stat() default 404 page");
// 	}
// 	response.http_version = "HTTP/1.1";
// 	response.status_code_phrase = "404 Not Found";
// 	response.header_fields["content-length"] = long_to_str(sb.st_size);
// 	assemble_header_str();
// }

#endif