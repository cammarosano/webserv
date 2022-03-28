#include "FdManager.hpp"

FdManager::FdManager() : capacity(10)
{
    fd_table = new fd_info[capacity];
    poll_array = new pollfd[capacity];

    // initialise poll_array
    for (int i = 0; i < capacity; i++)
    {
        poll_array[i].fd = -1;
        poll_array[i].events = 0;
    }

    // experimental
    poll_block = true;
}

FdManager::~FdManager() {
    delete[] poll_array;
    delete[] fd_table;
}

void FdManager::reallocate() {
    size_t new_capacity = 2 * capacity;

    // get new poll_array
    pollfd *new_poll_array = new pollfd[new_capacity];
    // copy old stuff
    for (int i = 0; i < capacity; i++) new_poll_array[i] = poll_array[i];
    // free the old one
    delete[] poll_array;
    // initialize new stuff
    for (size_t i = capacity; i < new_capacity; i++) {
        new_poll_array[i].fd = -1;
        new_poll_array[i].events = 0;
    }
    // get new fd_table (initialization already done by default constructor)
    fd_info *new_fd_table = new fd_info[new_capacity];
    // copy old stuff
    for (int i = 0; i < capacity; i++) new_fd_table[i] = fd_table[i];
    // free the old one
    delete[] fd_table;

    // update member variables
    capacity = new_capacity;
    poll_array = new_poll_array;
    fd_table = new_fd_table;
}

// returns maximum fd + 1
// use this as upper bound for looping through the poll array of fd_table
int FdManager::len() const {
    if (!fd_set.size()) return (0);
    int max_fd = *fd_set.rbegin();
    return max_fd + 1;
}

pollfd *FdManager::get_poll_array() { return (poll_array); }

void FdManager::add_listen_socket(int listen_socket,
                                  std::list<Vserver> &vservers) {
    if (listen_socket >= capacity) reallocate();

    fd_table[listen_socket].type = fd_listen_socket;

    poll_array[listen_socket].fd = listen_socket;
    poll_array[listen_socket].events = POLLIN;
    poll_array[listen_socket].revents = 0;

    vservers_map[listen_socket] = vservers;

    fd_set.insert(listen_socket);
}

void FdManager::add_client_socket(int client_socket, Client &client) {
    if (client_socket >= capacity) reallocate();

    fd_table[client_socket].type = fd_client_socket;
    fd_table[client_socket].client = &client;

    poll_array[client_socket].fd = client_socket;
    poll_array[client_socket].events = POLLIN;
    poll_array[client_socket].revents = 0;
    fd_set.insert(client_socket);
}

void FdManager::add_fd_read(int file_fd, Client &client) {
    while (file_fd >= capacity) reallocate();

    fd_table[file_fd].type = fd_read;
    fd_table[file_fd].client = &client;
    fd_table[file_fd].is_EOF = false;

    poll_array[file_fd].fd = file_fd;
    poll_array[file_fd].events = POLLIN;
    poll_array[file_fd].revents = 0;

    fd_set.insert(file_fd);
}

void	FdManager::add_fd_write(int fd, Client &client)
{
    while (fd >= capacity) reallocate();

    fd_table[fd].type = fd_write;
    fd_table[fd].client = &client;
    fd_table[fd].is_EOF = false;

    poll_array[fd].fd = fd;
    poll_array[fd].events = 0;
    poll_array[fd].revents = 0;

    fd_set.insert(fd);
}

// if fd was not added, it has no effect
void FdManager::remove_fd(int fd) {
    if (fd >= capacity) return;

    fd_table[fd].type = fd_none;
    fd_table[fd].client = NULL;
    fd_table[fd].is_EOF = false;

    poll_array[fd].fd = -1;
    poll_array[fd].events = 0;
    poll_array[fd].revents = 0;

    fd_set.erase(fd);
}

fd_info &FdManager::operator[](int fd) { return (fd_table[fd]); }

void FdManager::set_pollout(int fd) { poll_array[fd].events |= POLLOUT; }

void FdManager::unset_pollout(int fd) { poll_array[fd].events &= ~POLLOUT; }

void FdManager::unset_pollin(int fd) { poll_array[fd].events &= ~POLLIN; }

std::list<Vserver> &FdManager::get_vserver_lst(int listen_socket) {
    return vservers_map[listen_socket];
}

void FdManager::debug_info() const {
    static std::string types[6] = {"none         ", "listen_socket",
                                   "client_socket", "file_fd      ",
                                   "cgi_out_fd   ", "cgi_in_fd    "};

    std::cout << "----------FdManager debug info-----------------\n";
    for (int fd = 0; fd < len(); fd++) {
        if (poll_array[fd].fd == -1) continue;
        if (poll_array[fd].revents) std::cout << GREEN;
        std::cout << "fd " << fd << ": "
                  << "type " << types[fd_table[fd].type] << " | ";

        // monitored events
        if (poll_array[fd].events & POLLIN)
            std::cout << "monitoring for POLLIN | ";
        if (poll_array[fd].events & POLLOUT)
            std::cout << "monitoring for POLLOUT | ";

        // returned events
        if (poll_array[fd].revents & POLLIN)
            std::cout << "returned ready for POLLIN (read) | ";
        if (poll_array[fd].revents & POLLOUT)
            std::cout << "returned ready for POLLOUT (write) | ";

        // errors
        if (poll_array[fd].revents & POLLERR) std::cout << "POLLERR | ";
        if (poll_array[fd].revents & POLLHUP) std::cout << "POLLHUP | ";
        if (poll_array[fd].revents & POLLNVAL) std::cout << "POLLNVAL | ";

        std::cout << RESET << std::endl;
    }
    std::cout << "-----------------------------------------------\n";
}
