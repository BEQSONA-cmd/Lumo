#include "Lumo/Lumo.hpp"

Response TestRoute(Request req)
{
    return Response("This is a plain string response", 200, "OK");
}

int main()
{
    Lumo server("0.0.0.0", 8080);
    server.allowOrigins({"http://localhost:3000"});

    // route for testing plain string response
    server.registerRoute(TestRoute, "/test", "GET");

    server.start();
    return 0;
}
