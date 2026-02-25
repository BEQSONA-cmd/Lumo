#pragma once
#include "includes.hpp"

class WebSocketManager;

class WS
{
private:
    int socket_fd;
    std::atomic<bool> connected;
    std::string clientId;
    std::string route;
    WebSocketManager *manager;

public:
    WS(int fd, WebSocketManager *manager, const std::string &route);
    bool onMessage();
    bool onConnect() { return connected; }
    bool onClose() { return !connected; }
    bool isConnected() { return connected && running; }

    std::string getPayload();
    void sendPayload(const std::string &message);
    std::string getClientId() const { return clientId; }
    void broadcast(const std::string &message, const std::string &senderId);
};