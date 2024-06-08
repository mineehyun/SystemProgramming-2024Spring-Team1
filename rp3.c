#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "socket.c"

int main()
{
    char buffer[256] = "kimhyunmin";
    int sock = socket_client("192.168.12.4", DEFAULT_PORT);
    dprintf(sock, buffer);
    // read(sock, buffer, 256);
    printf("%s\n", buffer);
    return 0;
}