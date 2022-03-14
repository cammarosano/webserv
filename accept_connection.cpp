#include "FdManager.hpp"
#include "includes.hpp"

// accept() a new connection, create a Client object
// return  0 if ok, -1 if error
int accept_connection(int listen_socket, FdManager &table) {
    int client_socket;
    sockaddr client_addr;
    socklen_t addrlen = sizeof(client_addr);

    // accept new connection
    client_socket = accept(listen_socket, &client_addr, &addrlen);
    if (client_socket == -1) {
        perror("accept");
        return (-1);
    }

    // create Client
    Client *client =
        new Client(client_socket, table.get_vserver_lst(listen_socket));
    table.add_client_socket(client_socket, *client);

    // log to terminal
    std::cout << "Connection accepted. Client socket: " << client_socket
              << std::endl;

    return (0);
}
