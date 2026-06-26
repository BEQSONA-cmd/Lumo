#include "Lumo.hpp"

Lumo *globalServer = nullptr;
std::mutex app_mutex;

std::atomic<bool> running = true;

static void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

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
            error("Failed to create socket");

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
            error("Failed to bind socket");

        if (listen(server_fd, 10) < 0)
            error("Failed to listen on socket");
    }
}

void Lumo::useHttps(const std::string& certFile, const std::string& keyFile)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD* method = TLS_server_method();

    ssl_ctx = SSL_CTX_new(method);

    if (!ssl_ctx)
        throw std::runtime_error("Failed to create SSL_CTX");

    if (SSL_CTX_use_certificate_file(
            ssl_ctx,
            certFile.c_str(),
            SSL_FILETYPE_PEM) <= 0)
    {
        throw std::runtime_error("Failed loading certificate");
    }

    if (SSL_CTX_use_PrivateKey_file(
            ssl_ctx,
            keyFile.c_str(),
            SSL_FILETYPE_PEM) <= 0)
    {
        throw std::runtime_error("Failed loading private key");
    }

    if (!SSL_CTX_check_private_key(ssl_ctx))
    {
        throw std::runtime_error("Certificate/private key mismatch");
    }

    https_enabled = true;
}

SSL *Lumo::createSSL(int client_socket)
{
    SSL* ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, client_socket);

    if (SSL_accept(ssl) <= 0)
    {
        SSL_free(ssl);
        return nullptr;
    }

    return ssl;
}

void Lumo::worker(int new_socket)
{
    SSL* ssl = nullptr;

    if (https_enabled)
    {
        ssl = createSSL(new_socket);

        if (!ssl)
        {
            close(new_socket);
            return;
        }
    }

    char buffer[4096] = {0};
    int bytes;

    if (ssl)
        bytes = SSL_read(ssl, buffer, sizeof(buffer));
    else
        bytes = recv(new_socket, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        if (ssl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }

        close(new_socket);
        return;
    }

    Request req = req_manager.parseRequest(buffer);
    auto it = req.headers.find("Upgrade");
    if (it != req.headers.end() && it->second == "websocket")
    {
        ws_manager.handleWebSocket(new_socket, ssl, https_enabled, req);
        return;
    }
    Response res;

    {
        std::lock_guard<std::mutex> lock(app_mutex);
        res = req_manager.handleRequest(req);
    }

    std::string httpResponse = req_manager.buildHttpResponse(res, req);
    if (ssl)
        SSL_write(ssl, httpResponse.c_str(), httpResponse.size());
    else
        send(new_socket, httpResponse.c_str(), httpResponse.size(), 0);

    if (ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
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
