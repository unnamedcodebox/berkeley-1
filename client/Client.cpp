/** @file
 * @brief     Client declaration
 *
 * @ingroup   Berkeley
 *
 * @copyright (C) 2019
 */

#include "Client.h"

#include "../halifax/Socket.h"
#include <iostream>
#include <chrono>
#include <strings.h>
#include <cstring>
#include <vector>

#include <memory>

namespace berkeley
{

namespace
{

constexpr auto MESSAGE_MAX_LENGTH = 65537;

template <typename T>
void printMessage(const T& message)
{
    std::cout << std::string("[Message from server]: ") << message << "\n";
}

} // namespace

Client::Client(std::string serverAddress, unsigned int port, ProtocolType type)
    : m_address(serverAddress), m_port(port), m_type(type)
{
}

void Client::init()
{
    auto socket = m_type == ProtocolType::TCP
                      ? sockets::socket(AF_INET, SOCK_STREAM, 0)
                      : sockets::socket(AF_INET, SOCK_DGRAM, 0);
    auto serverAddress = sockaddr_in();

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(m_port);

    address::toBinary(AF_INET, m_address, &serverAddress.sin_addr);

    if (m_type == ProtocolType::TCP)
    {
        sockets::connect(
            socket,
            sockets_helpers::toSockaddrPointer(&serverAddress),
            sizeof(serverAddress));
    }

    auto processor = [socket, this, &serverAddress]()
    {
        char sendBuff[MESSAGE_MAX_LENGTH];
        char replyBuff[MESSAGE_MAX_LENGTH];

        while (true)
        {
            std::cin.getline(sendBuff, MESSAGE_MAX_LENGTH);

            if (m_type == ProtocolType::TCP)
            {
                sockets::send(socket, sendBuff, strlen(sendBuff), 0);

                bzero(replyBuff, sizeof(replyBuff));
                sockets::receive(socket, replyBuff, MESSAGE_MAX_LENGTH, 0);
            }
            else if (m_type == ProtocolType::UDP)
            {
                socklen_t length = 0;
                sockets::sendTo(
                    socket,
                    sendBuff,
                    strlen(sendBuff),
                    0,
                    sockets_helpers::toSockaddrPointer(&serverAddress),
                    sizeof(serverAddress));

                bzero(replyBuff, sizeof(replyBuff));

                sockets::receiveFrom(
                    socket,
                    replyBuff,
                    MESSAGE_MAX_LENGTH,
                    0,
                    sockets_helpers::toSockaddrPointer(&serverAddress),
                    &length);
            }
            printMessage(replyBuff);
        }
    };

    process(processor);
    if (m_type == ProtocolType::TCP)
    {
    close(socket);
    }
}

} // namespace berkeley
