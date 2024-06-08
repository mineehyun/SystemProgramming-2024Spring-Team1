#ifndef __SOCKET_H__
#define __SOCKET_H__

#define DEFAULT_PORT 8888

/**
 * Create a socket as server and wait connection.
 * Server socket will be closed automatically before return.
 *
 * @param port
 * 포트지뭐긴뭐야
 *
 * @returns
 * Client socket fd. -1 if error.
 */
int socket_server(uint32_t port);

/**
 * Create a socket as client and request connection.
 *
 * @param address
 * String type IPv4 address.
 * @param port
 * 포트지뭐긴뭐야
 *
 * @returns
 * Return the connected socket. -1 if error.
 */
int socket_client(char address[], uint32_t port);

#endif