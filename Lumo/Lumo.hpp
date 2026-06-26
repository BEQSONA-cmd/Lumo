#pragma once
#include "includes.hpp"
#include "WebSocketManager.hpp"
#include "RequestManager.hpp"

using json = nlohmann::json;

class Lumo
{
private:
    int port;
    std::string host;
    std::vector<std::thread> threads;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    WebSocketManager ws_manager;
    RequestManager req_manager;

    bool https_enabled = false;
    SSL_CTX *ssl_ctx = nullptr;

public:
    int server_fd;
    Lumo(std::string host, int port);

    void start();
    void worker(int client_socket);

    void useHttps(const std::string &certFile, const std::string &keyFile);
    SSL* createSSL(int client_socket);

    void registerRoute(Response (*handler)(Request), std::string path, std::string method)
    {
        req_manager.registerRoute(handler, path, method);
    }
    void registerWebSocketRoute(void (*handler)(WebSocket), std::string path)
    {
        ws_manager.registerRoute(handler, path);
    }
    void allowOrigins(std::vector<std::string> origins)
    {
        req_manager.allowOrigins(origins);
    }
    ~Lumo()
    {
        if (server_fd >= 0)
            close(server_fd);
    }
};