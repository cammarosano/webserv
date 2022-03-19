#include "includes.hpp"


// calls poll()
// does all read() and write() operations
void do_io(FdManager &table)
{
    int n_fds;

    // debug
    if (DEBUG)
        std::cout << "Blocking at poll()" << std::endl;

    // call poll()
    n_fds = poll(table.get_poll_array(), table.len(), -1);
    if (n_fds == -1)
    {
        perror("poll");
        return;
    }

    // display debug info
    if (DEBUG)
    {
        std::cout << "poll() returned " << n_fds << std::endl;
        table.debug_info();
    }

    // iterate over poll_array
    for (int fd = 3; fd < table.len() && n_fds; fd++)
    {
        short revents = table.get_poll_array()[fd].revents;
        if (!revents)
            continue;
        e_fd_type fd_type = table[fd].type;
        if (revents & POLLIN) // fd ready for reading
        {
            if (fd_type == fd_listen_socket)
                accept_connection(fd, table);
            else if (fd_type == fd_client_socket)
                recv_from_client(fd, table);
            else if (fd_type == fd_file)
                read_from_fd(fd, table);
        }
        if (revents & POLLOUT) // fd ready for writing
        {
            if (fd_type == fd_client_socket)
                send_to_client(fd, table);
            else if (fd_type == fd_cgi_input)
                write_to_fd(fd, table);
        }
        // when a process closes its end of the pipe, POLLHUP is detected
        if ((revents & (POLLIN | POLLHUP)) && fd_type == fd_cgi_output)
            read_from_fd(fd, table);
        --n_fds;
    }
}

// calls the respond() method of each request handler in
// the list. deletes request and request handler when
// the response is complete.
int handle_requests(std::list<ARequestHandler *> &list)
{
    std::list<ARequestHandler *>::iterator it;
    int ret;

    // iterate over list of request handlers
    it = list.begin();
    while (it != list.end())
    {
        ARequestHandler *req_handler = *it;
        // do response actions
        ret = req_handler->respond(); // subtype polymorphism
        if (ret == 0)                 // response not yet finished
            ++it;
        else
        {
            if (ret == 1) // finished successfully
                // update Client's state
                req_handler->getRequest()->client.state = recv_header;

            // free memory (request and req handler) and remove rh from list
            delete req_handler->getRequest();
            delete req_handler;
            it = list.erase(it); // returns iterator to next elem of list
        }
    }
    return (0);
}

int main(void)
{
    FdManager table;
    std::list<ARequestHandler *> req_handlers_lst;

    setup(table);
    while (1)
    {
        do_io(table);
        new_requests(table, req_handlers_lst);
        handle_requests(req_handlers_lst);
    }

    return (0);
}
