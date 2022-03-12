#include "ARequestHandler.hpp"
#include "FdManager.hpp"
#include "includes.hpp"

// calls poll()
// does all read() and write() operations
int do_io(FdManager &table) {
    int poll_ret;

    // debug
    std::cout << "Blocking at poll()" << std::endl;

    // call poll()
    poll_ret = poll(table.get_poll_array(), table.len(), -1);
    if (poll_ret == -1) {
        perror("poll");
        return (-1);
    }

    // iterate over poll_array
    for (int fd = 3; fd < table.len(); fd++) {
        if (table.get_poll_array()[fd].revents &
            POLLIN)  // fd ready for reading
        {
            // a the begining it will be the only the listening socket
            // other will be add with accept connection
            e_fd_type fd_type = table[fd].type;

            if (fd_type == fd_listen_socket) {
                accept_connection(fd, table);
            } else if (fd_type == fd_client_socket) {
                std::cout << "receiving data from client: " << fd << std::endl;
                recv_from_client(fd, table);
            } else if (fd_type == fd_file)
                read_from_file(fd, table); // consider renaming the function to read_from_fd
            else if (fd_type == fd_cgi_output)
            {
                read_from_file(fd, table);
                std::cout << "fd from cgi output " << fd << " is ready to read()" << std::endl;
            }
            
        }
        if (table.get_poll_array()[fd].revents &
            POLLOUT)  // fd ready for writing
        {
            e_fd_type fd_type = table[fd].type;
            std::cout << "sending data to client" << std::endl;
            if (fd_type == fd_client_socket) send_to_client(fd, table);
            // TODO: cgi_in
        }
    }
    return (0);
}

// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
int handle_requests(std::list<ARequestHandler *> &list) {
    std::list<ARequestHandler *>::iterator it;
    int ret;

    // iterate over list of request handlers
    it = list.begin();
    while (it != list.end()) {
        ARequestHandler *req_handler = *it;
        // do response actions
        ret = req_handler->respond();  // subtype polymorphism
        if (ret == 0)                  // response not yet finished
            ++it;
        else {
            if (ret == 1)  // finished successfully
                // update Client's state
                req_handler->getRequest()->client.state = recv_header;

            // free memory (request and req handler) and remove rh from list
            delete req_handler->getRequest();
            delete req_handler;
            it = list.erase(it);  // returns iterator to next elem of list
        }
    }
    return (0);
}

int main(void) {
    FdManager table;
    std::list<ARequestHandler *> req_handlers_lst;

    setup(table);
    while (1) {
        do_io(table);
        check4new_requests(table, req_handlers_lst);
        handle_requests(req_handlers_lst);
    }

    return (0);
}
