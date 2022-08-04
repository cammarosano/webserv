# webserv

A single threaded (thus, non-blocking) and somewhat RFC 7230 compliant HTTP server.

- Supported methods: GET, POST, HEAD  
- Text-based config file  
- Supports CGI (Python)  
- Supports multiple host:ports and multiple virtual servers in one IP:port  

### Build
`make`

### Run
`./webserv [config_file]`  

config_file is optional if you have one at conf/default.conf

## NOTES about recent changes:

### Clients state
- When a new connection is accept()ed, a Client object is created in IDLE state.
- When data is received from a IDLE client's socket, state is changed to INCOMING_REQUEST
- When a complete HTTP Request header is found at client's received_data buffer:
  - a new HTTPRequest object is instantiated,
  - a new Request Handler object is intantiated and
  - the client's state is changed to ONGOING_RESPONSE 
- When a response is finished, Client's state is changed back to:
  - IDLE if its received_data buffer is empty
  - INCOMING_REQUEST if there's (already) data in its received_data buffer
- Client's in a given state are kept in a list (Client class static variables). So, there are 3 lists, one for each state. And whenever a client's state changes, it is removed from one a list and inserted in another. List changes are done at constant time (O(1)) thanks the iterator *list_node* variable. Theses lists simplifies the loops in the program, namely: new_request(), handle_requests(), timeout() (in house_keep())

**TL;DR:** Clients cycle along 3 states: idle -> incoming_request -> ongoing_response -> idle -> etc... and are grouped in lists

### Time-outs
If a Client stays too long in the same state, it is timed-out (disconnected)

In detail:
- every time a client changes state, its *last_state_change* variable is updated to the current time, and the client is inserted at the back of the corresponding list. So the front of a list points to the oldest client in that state. That simplifies the following item a bit.
- the house_keeper function (main loop) checks for time-outs. Depending on the Client's state, *last_state_change* will be compared against CONNECTION_TIMEOUT (idle clients), REQUEST_TIMEOUT or RESPONSE_TIMEOUT

### Removing clients
remove_client() and the Client's destructor take care of updating the FdManager table and cleaning up the allocated resources. In case of a client in **ongoing_response** state, the Request Handler's destructor will be called (so the RH class should clear their resources as well: close fds, remove from table, wait for child_process without blocking)

### Protection against too many clients
- accept_connection does not accept new connections if number of clients is greater than MAX_CLIENTS. It tries to remove a Client in **idle** state to make room for the new client, or just makes the connection request waiting in the queue (I mean the listen() queue, handled by the kernel).
- when a AReqHandler (request handler) object is instantiated, if the number of clients is bigger than CONN_CLOSE_THRESHOLD * MAX_CLIENT (ex: 0.8 * MAX_CLIENT, that is, 80% of the maximum defined capacity), the response is tagged as **keep-alive = false**. That means that the HTTP response will have the header-field "Connection: close", which informs the client that the connection will be closed after the response is sent (in opposition to HTTP/1.1's default "Connection: keep-alive"). And the Client object is in fact removed when the response is over.



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

The structs **Vserver** and **Route** represent blocks in the configuration file. They are equivalent to nginx's "server" and "location" blocks. See conf/default.conf for an example.

### FdManager

The class **FdManager** ... well, manages file descriptors. And also organizes the array used for calling poll(). So an instance of this class, "table", is seen everywhere in the code. It allows us to know what a given file descriptor refers to a:
- listening socket
- client socket
- fd for read()ing from (a file in disk or the output of a CGI script)
- fd for writing to (a file in disk or the input for a CGI script).

And it also allow us to define whether a file descriptor will be polled for reading and/or writing operations.

### ARequestHandler

Base class for specific request handlers (like StaticRH and ErrorRH). A request handler performs a sequence of actions to respond to a request. Ex: assemble the header of the HTTP reponse, open a file in disk, set it up for reading operation in the IO round, close de fd when done, etc.

-----------------------------
An overview of the loop in main():

### The main loop

do_io(): does IO operations (no shit!!). Calls poll(), loops over the poll array and, depending on the type of file descriptor:

- accepts a new connection (creating a new Client object)
- receives data from a client socket
- sends data to a client socket
- reads data from a file in disk or pipe connected to CGI process
- send data to a file in disk (upload) or a pipe connected to a CGI process

new_requests():

- when a Client's received_data buffer contains the header of a new HTTP request, this data is parsed and an instance of a **HttpRequest** object is created. Vserver and Route resolution is done at construction.
- init_response() will instantiate the apropriate **RequestHandler**, after the resource path is determined (ex: /web_root/some_file.html). There are different types of RequestHandlers: one for serving a static file, one for generating an error page, one for generating the directory listing, one for handling a CGI response, etc. Each type is a specific class, but they all inherit from a base class **ARequestHandler** and define their own respond() method.

handle_request():

- iterates over list of clients in **ongoing_response** state and calls ARequestHandler's respond() method for each one. A cool subtype polymorphism is going on here (plz don't hate me).
- the respond() method of the RequestHandler will perform the sequence of steps in order to send a response to the client (ex: assemble the header of the HTTP reponse, open a file in disk, set it up of reading operation when IO is done, close de fd when done, etc.)
- most often times, a single call of a RequestHandler's repond() method does not complete the response, because it will depend on a IO operation (which can only be done after poll()ing). For this reason, the RequestHandler object keeps track of the **state** of the ongoing response, so it knows what to do when the response() method is called a second (or third, fourth...) time.
- When a reponse is complete, the HttpRequest object and RequestHandler object are deleted

This [diagram at miro](https://miro.com/app/board/uXjVOPebVU8=/?invite_link_id=956792833423) ilustrates a static file response.

[This](https://miro.com/welcomeonboard/MUJub3YwcDIwUkZMd3Eyb1FhdWUxN3NGeENrd0tGQUh4Q3Z6SHdJcnI4ek5zMThNUDJzejJEaHd3QVZ1a2dVc3wzNDU4NzY0NTE4MjMwNTU0NTUz?invite_link_id=398884532576) is a link if you wish to collaborate

------------
