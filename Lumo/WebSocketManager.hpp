#pragma once
#include "includes.hpp"
#include "WebSocket.hpp"
#include "RequestManager.hpp"

class WebSocketManager
{
private:
    std::map<std::string, std::vector<WebSocket>> route_clients;
    std::mutex clients_mutex;

public:
    std::map<std::string, void (*)(WebSocket)> ws_routes;
    void registerRoute(void (*handler)(WebSocket), std::string path)
    {
        ws_routes[path] = handler;
    }
    void handleWebSocket(int client_socket, Request &req);
    void add(WebSocket client, const std::string &path);
    void broadcast(const std::string &message, const std::string &senderId, const std::string &route);
    void remove(const std::string &clientId);
};