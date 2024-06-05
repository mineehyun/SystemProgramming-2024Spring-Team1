#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "socket.h"

int create_socket_server(int port)
{
    printf("[Server] Creating socket port %d\n", port);
    int server_sock, client_sock;
    struct sockaddr_in server_address = 
    {
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_family = AF_INET,
        .sin_port = port,
    };
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("[Server] Socket failed");
        return -1;
    }
    if (bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("[Server] Bind failed");
        close(server_sock);
        return -1;
    }
    if (listen(server_sock, 5) == -1)
    {
        perror("[Server] Listen failed");
        close(server_sock);
        return -1;
    }
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_address_len);
    if (client_sock == -1)
    {
        perror("[Server] Accept failed");
        close(server_sock);
        return -1;
    }
    close(server_sock);
    printf("[Server] Created socket port %d\n", port);
    return client_sock;
}

int create_socket_client(char address[], int port)
{
    printf("[Client] Creating socket address %s port %d\n", address, port);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = 
    {
        .sin_addr.s_addr = inet_addr(address),
        .sin_family = AF_INET,
        .sin_port = port,
    };
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("[Client] Connect failed");
        close(sock);
        return -1;
    }
    printf("[Client] Created socket address %s port %d\n", address, port);
    return sock;
}