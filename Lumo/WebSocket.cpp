#include "WebSocket.hpp"
#include "WebSocketManager.hpp"

WS::WS(int fd, WebSocketManager *manager, const std::string &route)
{
    socket_fd = fd;
    connected = true;
    clientId = std::to_string(fd);
    this->manager = manager;
    this->route = route;
}

bool WS::onMessage()
{
    if (!connected)
        return false;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket_fd, &readfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = MSG_TIMEOUT * 1000;

    int activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);

    if (activity < 0)
    {
        connected = false;
        return false;
    }

    return activity > 0;
}

std::string WS::getPayload()
{
    unsigned char buffer[4096];
    int bytes = recv(socket_fd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        connected = false;
        return "";
    }

    if ((buffer[0] & 0x0F) == 0x8)
    {
        connected = false;
        return "";
    }

    int payload_len = buffer[1] & 127;
    int mask_offset = 2;

    if (payload_len == 126)
        mask_offset = 4;
    else if (payload_len == 127)
        mask_offset = 10;

    unsigned char *masking_key = buffer + mask_offset;
    unsigned char *payload = buffer + mask_offset + 4;

    std::string message;
    for (int i = 0; i < payload_len; i++)
    {
        message += payload[i] ^ masking_key[i % 4];
    }

    return message;
}

void WS::sendPayload(const std::string &message)
{
    if (!connected)
        return;

    std::vector<unsigned char> frame;

    frame.push_back(0x81);

    if (message.size() <= 125)
    {
        frame.push_back(message.size());
    }
    else if (message.size() <= 65535)
    {
        frame.push_back(126);
        frame.push_back((message.size() >> 8) & 0xFF);
        frame.push_back(message.size() & 0xFF);
    }
    else
    {
        frame.push_back(127);
        uint64_t len = message.size();
        for (int i = 7; i >= 0; i--)
        {
            frame.push_back((len >> (i * 8)) & 0xFF);
        }
    }

    frame.insert(frame.end(), message.begin(), message.end());

    if (send(socket_fd, frame.data(), frame.size(), 0) < 0)
    {
        connected = false;
    }
}

void WS::broadcast(const std::string &message, const std::string &senderId)
{
    manager->broadcast(message, senderId, route);
}