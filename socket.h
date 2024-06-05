#ifndef __SOCKET_H__
#define __SOCKET_H__

#define DEFAULT_PORT 8888

/**
 * create_socket_server(@port)
 * DESCRIPTION
 *  Create a socket as server and wait for connection.
 *  Socket must be closed by `close`.
 *  Note that server socket will be closed before function return.
 *  So cannot use same server socket to accept two or more clients.
 * 
 * RETURN
 *  Return the connected socket. Negative means error.
 */
int create_socket_server(int port);

/**
 * create_socket_client(@address, @port)
 * DESCRIPTION
 *  Create a socket as client and request connection to @address @port.
 *  Socket must be closed by `close`.
 * 
 * RETURN
 *  Return the connected socket. Negative means error.
 */
int create_socket_client(char address[], int port);

#endif