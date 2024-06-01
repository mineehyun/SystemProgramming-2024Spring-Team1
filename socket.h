#ifndef __SOCKET_H__
#define __SOCKET_H__

#define SOCKET_SOCKET_ERRNO -1
#define SOCKET_BIND_ERRNO -2
#define SOCKET_LISTEN_ERRNO -3
#define SOCKET_ACCEPT_ERRNO -4
#define SOCKET_CONNECT_ERRNO -5

#define DEFAULT_PORT 8888

/**
 * create_socket_server(@port)
 * Create a socket as server and wait for connection.
 * Socket must be closed by `close`.
 * 
 * RETURN
 * Return the connected socket. Negative means error.
 */
int create_socket_server(int port);

/**
 * create_socket_client(@address, @port)
 * Create a socket as client and request connection to @address @port.
 * Socket must be closed by `close`.
 * 
 * RETURN
 * Return the connected socket. Negative means error.
 */
int create_socket_client(char address[], int port);

#endif