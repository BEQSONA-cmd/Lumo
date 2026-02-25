#include "RequestManager.hpp"

std::string RequestManager::buildHttpResponse(const Response &res, const Request &req)
{
    std::ostringstream response;

    response << "HTTP/1.1 " << res.statusCode << " " << res.statusMessage << "\r\n";

    auto it = req.headers.find("Origin");
    if (it != req.headers.end())
    {
        const std::string &origin = it->second;

        for (const auto &allowed : origins)
        {
            if (origin == allowed)
            {
                response << "Access-Control-Allow-Origin: " << origin << "\r\n";
                response << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
                response << "Access-Control-Allow-Headers: Content-Type\r\n";
                response << "Access-Control-Allow-Credentials: true\r\n";
                break;
            }
        }
    }

    response << "Content-Type: application/json\r\n";
    response << "Content-Length: " << res.body.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << res.body;

    return response.str();
}

Response RequestManager::handleRequest(const Request &req)
{
    if (req.method == "OPTIONS")
        return Response("", 200, "OK");
    for (const auto &route : routes)
    {
        if (route.method == req.method && req.path.find(route.path) != std::string::npos)
            return route.handler(req);
    }

    return Response("Not Found", 404, "Not Found");
}

Request RequestManager::parseRequest(const std::string &raw)
{
    std::istringstream stream(raw);
    std::string line;

    Request req;

    std::getline(stream, line);
    std::istringstream firstLine(line);
    firstLine >> req.method >> req.path;

    while (std::getline(stream, line) && line != "\r")
    {
        size_t colon = line.find(":");
        if (colon != std::string::npos)
        {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 2);
            value.erase(value.find("\r"));
            req.headers[key] = value;
        }
    }

    std::string body;
    while (std::getline(stream, line))
        body += line;

    req.body = body;
    return req;
}
