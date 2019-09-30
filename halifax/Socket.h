/** @file
 * @brief     Socket declaration
 *
 * @ingroup   Berkeley
 *
 * @copyright (C) 2019
 */

#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>

namespace berkeley
{

/**
 * Wrappers around berkeley sockets functions
 */
namespace sockets
{

/**
 * Returns socket descriptor
 */
int socket(int family, int type, int protocol);

/**
 * Bind socket to local protocol address
 */
int bind(int socket, sockaddr* address, socklen_t length);

/**
 * Listen socket
 */
void listen(int socket, int backlog);

/**
 * Connect client to server
 */
void connect(
    int socket, const struct sockaddr* serverAddress, socklen_t addressLength);

/**
 * Returns connected socket
 */
int accept(
    int listeningSocket, sockaddr* clientAddress, socklen_t* addressLength);

/**
 * Receive message from socket
 */
int receive(int socket, void* buffer, size_t length, int flags);

/**
 * Receive message from socket
 */
int receiveFrom(
    int socket,
    void* buffer,
    size_t length,
    int flags,
    sockaddr* address,
    socklen_t* socketLength);

/**
 * Send message to socket
 */
int send(int socket, const void* buffer, size_t length, int flags);

/**
 * Send message to socket
 */
int sendTo(
    int socket,
    void* buffer,
    size_t length,
    int flags,
    sockaddr* address,
    socklen_t socketLength);

} // namespace sockets

namespace sockets_helpers
{

sockaddr* toSockaddrPointer(sockaddr_in* addr);

/**
 * Create socket address
 */
sockaddr_in createSocketAddress(int port);

} // namespace sockets_helpers

namespace address
{

void toBinary(int family, std::string ipAddr, void* address);

} // namespace address

namespace string_utils
{

std::string stringFromBuffer(const char* buffer);

} // namespace string_utils

} // namespace berkeley
