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
        std::cout << "max fd number: " << table.len() << std::endl;
    }

    time_t current_time = time(NULL);

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
                accept_connection(fd, table, current_time);
            else if (table[fd].type == fd_client_socket)
                recv_from_client(fd, table, current_time);
            else if (table[fd].type == fd_read)
                read_from_fd(fd, table);
        }
        if (revents & POLLOUT) // fd ready for writing
        {
            if (table[fd].type == fd_client_socket)
                send_to_client(fd, table, current_time);
            else if (table[fd].type == fd_write)
                write_to_fd(fd, table);
        }
    }
}

bool stop = false; // evil global var
void signal_handler(int) {stop = true;}

int main(int argc, char** argv)
{
    FdManager table;

    signal(SIGINT, signal_handler);
    if (setup(table, argc, argv) == -1)
        return (1);
    while (!stop)
    {
        do_io(table);
        new_requests(table);
        handle_requests(table);
        reaper(table);
    }
    clear_resources(table);
    return (0);
}
