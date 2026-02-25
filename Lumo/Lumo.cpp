#include "Lumo.hpp"

Lumo *globalServer = nullptr;
std::mutex app_mutex;

std::atomic<bool> running = true;

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        running = false;
        if (globalServer)
            close(globalServer->server_fd);
    }
}

Lumo::Lumo(std::string host, int port)
{
    this->host = host;
    this->port = port;

    OPENSSL_init_ssl(0, NULL);
    OPENSSL_init_crypto(0, NULL);

    globalServer = this;
    std::signal(SIGINT, signalHandler);

    {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        if (server_fd <= 0)
            exit(EXIT_FAILURE);

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
            exit(EXIT_FAILURE);

        if (listen(server_fd, 10) < 0)
            exit(EXIT_FAILURE);
    }
}

void Lumo::worker(int new_socket)
{
    char buffer[4096] = {0};
    read(new_socket, buffer, 4096);

    Request req = req_manager.parseRequest(buffer);
    auto it = req.headers.find("Upgrade");
    if (it != req.headers.end() && it->second == "websocket")
    {
        ws_manager.handleWebSocket(new_socket, req);
        return;
    }
    Response res;

    {
        std::lock_guard<std::mutex> lock(app_mutex);
        res = req_manager.handleRequest(req);
    }

    std::string httpResponse = req_manager.buildHttpResponse(res, req);
    send(new_socket, httpResponse.c_str(), httpResponse.size(), 0);

    close(new_socket);
}

void Lumo::start()
{
    std::cout << "Lumo server running at http://" << host << ":" << port << "\n";

    while (running)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (new_socket < 0)
            continue;

        threads.emplace_back(&Lumo::worker, this, new_socket);
    }

    for (auto &thread : threads)
        if (thread.joinable())
            thread.join();
}
