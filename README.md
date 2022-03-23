# webserv

## TODO

-  ### configuration file parser

    - ### parse server block
      - [x] listening port
      - [x] server names
      - [x] error pages
      - [ ] body size limit (accept other than M ?)
    - ### parse location block
      - [x] root
      - [x] auto_index
      - [x] default index file
      - [x] cgi extension
      - [x] cgi interpreter
      - [x] accepted method
      - [ ] upload accepted
      - [ ] upload dir
      - [x] redirection


- ### request handler class for Redirections
  - [x] redirect server
  - [ ] redirect location

- ### request handler class for Directory listing

  - [x] generates an HTML page with the directory listing (auto-index)
  - [ ] display folder first then files (optional)

- ### "Try to always use the most "C++" code possible (for example use <cstring> over <string.h>)."

- ### Memory clean up when exiting program

- ### Header fields in the Http response:
  - content-type
  - date

- ## TESTING

  - siege
  - subject's tester (not mandatory to pass this test)
  - curl, postman, etc
  - Conditions:
    - hanging requests (sends only part of the header)
    - hanging requests (sends header but only part of the body)

- ### check also "Issues" in the repo

-------------------

## Resources

(credits to David for most of the links below)

Nginx configuration file,
 route (they call it "location") resolution:
[Nginx beginners guide](https://nginx.org/en/docs/beginners_guide.html)

Nginx virtual server resolution:
[How Nginx processes a request](https://nginx.org/en/docs/http/request_processing.html)

Gentle intro to socket programming and HTTP:
[HTTP Server: Everything you need to know to Build a simple HTTP server from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

A more complete guide to sockets/network programming:
[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)

HTTP tuto if you're still scared of diving into the RFCs:
[HTTP Made Really Easy](https://www.jmarshall.com/easy/http/)

And CGI! (short read)
[CGI Made Really Easy](https://www.jmarshall.com/easy/cgi/)

Asking ~~Paul~~ poll() which file descriptors are ready for reading/writing:
[IBM - Using poll()](https://www.ibm.com/docs/en/i/7.2?topic=designs-using-poll-instead-select)

The real deal:
[RFC 7230](https://datatracker.ietf.org/doc/html/rfc7230)

------------------------

## Program design

I suggest you take a look at the contents of the **includes** directory before you read the code starting from main.cpp. Here's a brief description of some classes/structs:

### Client

A **Client** struct represents an accepted connection. It contains a socket and 3 buffers for IO operations: (see includes.hpp):

- received_data: stores data read() from the client socket
- unsent_data: stores data that must be sent to client socket when that socket is ready for a write() operation
- decoded_body: stores the body of a request after removing chunking meta-data, to be sent to a CGI process or a file in disk (when uploading).

### Vserver and Route

The structs **Vserver** and **Route** represent blocks in the configuration file. They are equivalent to nginx's "server" and "location" blocks. See includes.hpp for their definition, and get_test_config (setup.cpp) for an example.

### FdManager

The class **FdManager** ... well, manages file descriptors. And also organizes the array used for calling poll(). So an instance of this class, "table", is seen everywhere in the code. It allows us to know what a given file descriptor refers to (a listening socket, a client socket, a file in disk, the output of a CGI script, etc). And also allow us to define whether a file descriptor will be polled for reading and/or writing operations.

### ARequestHandler

Base class for specific request handlers (like StaticRH and ErrorRH). A request handler performs a sequence of actions to respond to a request. Ex: assemble the header of the HTTP reponse, open a file in disk, set it up of reading operation when IO is done, close de fd when done, etc.

-----------------------------
An overview of the loop in main():

### The main loop

do_io(): does IO operations (no shit!!). Calls poll(), loops over the poll array and, depending on the type of file descriptor:

- accepts a new connection (creating a new Client object)
- receives data from a client socket
- sends data to a client socket
- reads data from a file in disk or pipe connected to CGI process
- send data to a file in disk (upload) or a pipe connected to a CGI process

-new_requests():

- when a Client's received_data buffer contains the header of a new HTTP request, this data is parsed and an instance of a **HttpRequest** object is created.
- After resolving which Vserver and Route applies to this request, the resource path is determined (ex: /web_root/some_file.html), and the apropriate **RequestHandler** is instantiated and added to a list. There are different types of RequestHandlers: one for serving a static file, one for generating an error page, one for generating the directory listing, one for handling a CGI response, etc. Each type is a specific class, but they all inherit from a base class **ARequestHandler** and define their own respond() method.

handle_request():

- iterates over the list of *ARequestHandler (pointers to an abstract class) and calls the respond() method for each one. The respond() method of the specific class is called (yep, subtype polymorphism. plz don't hate me).
- the respond() method of the RequestHandler will perform the sequence of steps in order to send a response to the client (ex: assemble the header of the HTTP reponse, open a file in disk, set it up of reading operation when IO is done, close de fd when done, etc.)
- most often times, a single call of a RequestHandler's repond() method does not complete the response, because it will depend on a IO operation (which can only be done after poll()ing). For this reason, the RequestHandler object keeps track of the **state** of the ongoing response, so it knows what to do when the response() method is called a second (or third, fourth...) time.
- When a reponse is complete, the HttpRequest object and RequestHandler are deleted and removed from the list

This [diagram at miro](https://miro.com/app/board/uXjVOPebVU8=/?invite_link_id=956792833423) ilustrates a static file response.

[This](https://miro.com/welcomeonboard/MUJub3YwcDIwUkZMd3Eyb1FhdWUxN3NGeENrd0tGQUh4Q3Z6SHdJcnI4ek5zMThNUDJzejJEaHd3QVZ1a2dVc3wzNDU4NzY0NTE4MjMwNTU0NTUz?invite_link_id=398884532576) is a link if you wish to collaborate

------------