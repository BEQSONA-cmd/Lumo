#pragma once
#include "includes.hpp"

struct Request
{
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

class Response
{
public:
    std::string body;
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;

    Response(std::string body, int statusCode, std::string statusMessage)
        : body(body), statusCode(statusCode), statusMessage(statusMessage) {}
    Response() : body(""), statusCode(200), statusMessage("OK") {}
    ~Response() {}
};

struct RouteHandler
{
    Response (*handler)(Request);
    std::string path;
    std::string method;
};

class RequestManager
{
private:
    std::vector<std::string> origins;
    std::vector<RouteHandler> routes;

public:
    void registerRoute(Response (*handler)(Request), std::string path, std::string method)
    {
        routes.push_back({handler, path, method});
    }
    void allowOrigins(std::vector<std::string> origins)
    {
        this->origins = origins;
    }
    std::string buildHttpResponse(const Response &res, const Request &req);
    Response handleRequest(const Request &req);
    Request parseRequest(const std::string &raw);
};