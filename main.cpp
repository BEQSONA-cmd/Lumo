#include "Lumo/Lumo.hpp"

class User
{
public:
    std::string username;
    std::string email;
    int age;
};
// after creating the class you have to define the DTO type,
// only if its used as a request body or response body
DTO(User, username, email, age);

User globalUser;

// the Response can handle 4 types of body
// 1. Object
Response GetUserRoute(Request req)
{
    return Response(globalUser, 200, "OK");
}

// 2. JSON object
Response GetUserRoute2(Request req)
{
    return Response(json(globalUser), 200, "OK");
}

// 3. String format JSON
Response GetUserRoute3(Request req)
{
    return Response(json(globalUser).dump(), 200, "OK");
}

// 4. Plain string
Response TestRoute(Request req)
{
    return Response("This is a plain string response", 200, "OK");
}

Response RegisterUserRoute(Request req)
{
    // json::parse() will get the JSON from the request body and convert it to a json object
    // json j = json::parse(req.body);

    // then we can use the get() method to convert the json object to a User object
    // User newUser = j.get<User>();

    // or just do it in one line
    globalUser = json::parse(req.body).get<User>();

    return Response("User registered successfully", 201, "OK");
}

// websocket example
void chat(WebSocket ws)
{
    while (ws->isConnected())
    {
        if (ws->onMessage())
        {
            std::string msg = ws->getPayload();
            ws->broadcast(msg, ws->getClientId());
        }
    }
}

int main()
{
    Lumo server("0.0.0.0", 8080);
    server.allowOrigins({"http://localhost:3000"});

    // route for testing plain string response
    server.registerRoute(TestRoute, "/test", "GET");

    // route for testing JSON response
    server.registerRoute(GetUserRoute, "/user", "GET");
    server.registerRoute(GetUserRoute2, "/user2", "GET");
    server.registerRoute(GetUserRoute3, "/user3", "GET");

    // route for testing JSON request body
    server.registerRoute(RegisterUserRoute, "/user", "POST");

    // websocket route
    server.registerWebSocketRoute(chat, "/chat");

    server.start();
    return 0;
}
