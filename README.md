# webserv

What it does at this point:
- serve static files (GET requests)
- supports multiple ports, (virtual) servers and routes. (configuration is hard coded for testing, no parsing of a config file yet)

Not much error checking/handling. Http response headers very incomplete.
<br>
<br>
<br>
### What is does:
![img](web_root/imgs/get_static.png)

### What it shall do:
![img](web_root/imgs/all.png)


## Resources

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

