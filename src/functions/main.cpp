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
    n_fds = poll(table.get_poll_array(), table.len(), POLL_TIME_OUT);
    // n_fds = poll(table.get_poll_array(), table.len(), -1);
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
        --n_fds;
        if ((revents & (POLLIN | POLLHUP))) // fd ready for reading
        {
            if (table[fd].type == fd_listen_socket)
                accept_connection(fd, table);
            else if (table[fd].type == fd_client_socket)
                recv_from_client(fd, table);
            else if (table[fd].type == fd_read)
                read_from_fd(fd, table);
        }
        if (revents & POLLOUT) // fd ready for writing
        {
            if (table[fd].type == fd_client_socket)
                send_to_client(fd, table);
            else if (table[fd].type == fd_write)
                write_to_fd(fd, table);
        }
    }
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
        handle_requests(req_handlers_lst, table);
    }

    return (0);
}
