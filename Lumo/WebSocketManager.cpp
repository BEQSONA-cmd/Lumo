#include "WebSocketManager.hpp"

std::string generateWebSocketAccept(const std::string &key)
{
    std::string magic = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)magic.c_str(), magic.size(), hash);

    char base64[256];
    EVP_EncodeBlock((unsigned char *)base64, hash, SHA_DIGEST_LENGTH);

    return std::string(base64);
}

void WebSocketManager::handleWebSocket(int client_socket, Request &req)
{
    std::string key = req.headers["Sec-WebSocket-Key"];
    std::string accept = generateWebSocketAccept(key);

    std::ostringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << accept << "\r\n\r\n";

    send(client_socket, response.str().c_str(), response.str().size(), 0);

    if (ws_routes.find(req.path) == ws_routes.end())
    {
        close(client_socket);
        return;
    }

    auto ws = std::make_shared<WebSocket>(client_socket, this, req.path);

    add(ws, req.path);

    ws_routes[req.path](ws);

    remove(ws->getClientId());
}

void WebSocketManager::add(std::shared_ptr<WebSocket> client, const std::string &path)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    route_clients[path].push_back(client);
}

void WebSocketManager::broadcast(const std::string &message,
                                 const std::string &senderId,
                                 const std::string &route)
{
    std::lock_guard<std::mutex> lock(clients_mutex);

    for (auto &client : route_clients[route])
    {
        if (client->getClientId() != senderId && client->isConnected())
        {
            client->sendPayload(message);
        }
    }
}

void WebSocketManager::remove(const std::string &clientId)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto &route : route_clients)
    {
        auto &clients = route.second;
        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                     [&](std::shared_ptr<WebSocket> client)
                                     { return client->getClientId() == clientId; }),
                      clients.end());
    }
}