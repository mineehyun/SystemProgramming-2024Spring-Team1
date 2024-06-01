#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "socket.h"

int create_socket_server(int port)
{
    int server_sock, client_sock;
    struct sockaddr_in server_address = 
    {
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_family = AF_INET,
        .sin_port = port,
    };
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) return SOCKET_SOCKET_ERRNO;
    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) return SOCKET_BIND_ERRNO;
    if (listen(server_sock, 5) == -1) return SOCKET_LISTEN_ERRNO;
    /* Wait for connection request */
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_len);
    if (client_sock == -1) return SOCKET_ACCEPT_ERRNO;
    return client_sock;
}

int create_socket_client(char address[], int port)
{
    int sock;
    struct sockaddr_in server_address = 
    {
        .sin_addr.s_addr = inet_addr(address),
        .sin_family = AF_INET,
        .sin_port = port,
    };
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) return SOCKET_CONNECT_ERRNO;
    return sock;
}