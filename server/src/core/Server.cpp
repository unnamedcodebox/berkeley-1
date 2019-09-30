/** @file
 * @brief     Server implementation
 *
 * @ingroup   Berkeley
 *
 * @copyright (C) 2019
 */

#include "Server.h"
#include "../core/MessageProcessor.h"

#include "../../../halifax/Socket.h"

#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include <iostream>
#include <signal.h>

namespace berkeley
{

namespace
{

constexpr auto MESSAGE_MAX_LENGTH = 65537;

socklen_t socketLength(const sockaddr_in& address)
{
    return static_cast<socklen_t>(sizeof(address));
}

void copyStringToBuffer(char* buffer, const std::string& string)
{
    strcpy(buffer, string.c_str());
}

std::string processString(const std::string& protocol,
                          const std::string& received)
{
    auto messageProcessor = MessageProcessor(received);
    auto processed = protocol + " | " + received + "\n" + messageProcessor();

    return processed;
}

} // namespace

Server::Server(unsigned int port) : m_port(port) {}

void Server::init()
{
    auto LISTENQ = 10;
    void sig_chld(int);
    pid_t childPid;

    auto listenedSocket = sockets::socket(AF_INET, SOCK_STREAM, 0);
    auto clientAddress = sockaddr_in();
    auto serverAddress = sockets_helpers::createSocketAddress(m_port);

    sockets::bind(
        listenedSocket,
        sockets_helpers::toSockaddrPointer(&serverAddress),
        sizeof(serverAddress));
    sockets::listen(listenedSocket, LISTENQ);

    auto udpSocket = sockets::socket(AF_INET, SOCK_DGRAM, 0);
    sockets::bind(
        udpSocket,
        sockets_helpers::toSockaddrPointer(&serverAddress),
        sizeof(serverAddress));

    fd_set rset;
    FD_ZERO(&rset);
    auto maxfdp = std::max(listenedSocket, udpSocket) + 1;
    signal(SIGCHLD, SIG_IGN);

    while(true)
    {
        FD_SET(listenedSocket, &rset);
        FD_SET(udpSocket, &rset);
        select(maxfdp, &rset, nullptr, nullptr, nullptr);

        if (FD_ISSET(listenedSocket, &rset))
        {
            auto clientLength = socketLength(clientAddress);
            auto connectedSocket = accept(
                listenedSocket,
                sockets_helpers::toSockaddrPointer(&clientAddress),
                &clientLength);

            childPid = fork();

            if (childPid == 0)
            {
                close(listenedSocket);
                auto processor = [connectedSocket]()
                {
                    char buffer[MESSAGE_MAX_LENGTH];
                    while (true)
                    {
                        bzero(buffer, sizeof(buffer));

                        sockets::
                            receive(connectedSocket, buffer, MESSAGE_MAX_LENGTH, 0);

                        auto received = string_utils::stringFromBuffer(buffer);
                        auto processed = processString("TCP", received);

                        copyStringToBuffer(buffer, processed);
                        sockets::
                            send(connectedSocket, buffer, processed.length(), 0);
                    }
                };
                process(processor);
                close(connectedSocket);
                exit(0);
            }
            else if (childPid == -1)
            {
                std::cerr << "FORK ERROR";
            }
            close(connectedSocket);
        }
        if (FD_ISSET(udpSocket, &rset))
        {
            char buffer[MESSAGE_MAX_LENGTH];

            auto clientLength = socketLength(clientAddress);

            bzero(buffer, sizeof(buffer));
            sockets::receiveFrom(
                udpSocket,
                buffer,
                MESSAGE_MAX_LENGTH,
                0,
                sockets_helpers::toSockaddrPointer(&clientAddress),
                &clientLength);

            auto received = string_utils::stringFromBuffer(buffer);

            auto processed = processString("UDP", received);

            copyStringToBuffer(buffer, processed);

            sockets::sendTo(
                udpSocket,
                buffer,
                processed.length(),
                0,
                sockets_helpers::toSockaddrPointer(&clientAddress),
                clientLength);
        }
    }
}

} // namespace berkeley
