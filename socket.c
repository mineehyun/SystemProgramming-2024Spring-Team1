#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "socket.h"

int socket_server(uint32_t port)
{
    /* Server socket */
    struct sockaddr_in server_address = {0};
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("[socket_server] socket");
        return -1;
    }
    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("[socket_server] bind");
        close(server_sock);
        return -1;
    }
    if (listen(server_sock, 5) == -1)
    {
        perror("[socket_server] listen");
        close(server_sock);
        return -1;
    }
    /* Client socket */
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_len);
    if (client_sock == -1)
    {
        perror("[socket_server] accept");
        close(server_sock);
        return -1;
    }
    close(server_sock);
    return client_sock;
}

int socket_client(char address[], uint32_t port)
{
    struct sockaddr_in server_address = {0};
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("[socket_client] socket");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("[socket_client] connect");
        close(sock);
        return -1;
    }
    return sock;
}