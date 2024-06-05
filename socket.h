#ifndef __SOCKET_H__
#define __SOCKET_H__

#define DEFAULT_PORT 8888

/**
 * @brief
 * Create a socket as server and wait for connection.
 * Socket must be closed.
 * @param port
 * 포트지뭐긴뭐야
 * @return
 * Return the connected client socket. -1 if error.
 */
int create_socket_server(int port);

/**
 * @brief
 * Create a socket as client and request connection.
 * Socket must be closed.
 * @param address
 * String type IPv4 address
 * @param port
 * 포트지뭐긴뭐야
 * @return
 * Return the connected socket. -1 if error.
 */
int create_socket_client(char address[], int port);

#endif