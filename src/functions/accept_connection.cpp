#include "includes.hpp"

void refuse_connection(int listen_socket)
{
    int client_socket;
    sockaddr client_addr;
    socklen_t addrlen = sizeof(client_addr);

    client_socket = accept(listen_socket, &client_addr, &addrlen);
    close (client_socket);
    std::cout << "Connection refused (max num clients reached)" 
        << std::endl;
}

// returns 1 if OK, new client can be accepted
// returns 0 otherwise
int make_room_for_new_client(FdManager &table, time_t now)
{
    if (Client::idle_clients.empty())
        return (0);

    Client &oldest_idle_client = *Client::idle_clients.back();
    // clients idle for less the MIN_IDLE_TIME shall not be disconnected
    if (difftime(now, oldest_idle_client.last_io) < MIN_IDLE_TIME)
        return (0);
    remove_client(oldest_idle_client, table,
                    "webserv (max num clients reached)");
    return (1);
}

// accept() a new connection, create a Client object
void accept_connection(int listen_socket, FdManager &table, time_t now)
{
    int client_socket;
    sockaddr client_addr;
    socklen_t addrlen = sizeof(client_addr);

    if (Client::counter > MAX_CLIENTS)
    {
        if (!make_room_for_new_client(table, now))
        {
            // refuse_connection(listen_socket);
            return; // make it wait
        }
    }

    // accept new connection
    client_socket = accept(listen_socket, &client_addr, &addrlen);
    if (client_socket == -1) {
        perror("accept");
        return;
    }

    // create Client
    Client *client = new Client(client_socket,
                        client_addr, table.get_vserver_lst(listen_socket));
    table.add_client_socket(client_socket, *client);

    // log to terminal
    if (DEBUG)
    std::cout << "Connection accepted. Client socket: " << client_socket
              << std::endl;
}
