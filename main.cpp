#include "Lumo/Lumo.hpp"

Response helloRoute(Request req)
{
    (void)req; // unused
    return Response("Hello World!", 200, "OK");
}

int main()
{
    Lumo server("0.0.0.0", 8080);
    server.allowOrigins({"http://localhost:3000"});
    server.registerRoute(helloRoute, "/hello", "GET");
    server.start();
    return 0;
}