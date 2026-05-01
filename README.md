# Lumo

A lightweight, multithreaded HTTP & WebSocket engine built in modern C++.

## Features

- No giant dependencies.
- Just raw sockets, threads, and clean routing.
- HTTP route handling
- WebSocket routes
- Thread-per-connection model
- CORS allow origins
- JSON support with `nlohmann/json`
- Easy route registration
- Broadcast support for WebSockets

---

## 🧠 How It Works

```
Incoming TCP Connection
          ↓
       accept()
          ↓
std::thread (per connection)
          ↓
 HTTP/WebSocket Manager
          ↓
    Route Handler
```

## 🧵 Multithreading Model

Lumo uses a **thread-per-connection architecture**.

Each incoming connection:

1. `accept()` is called
2. A new `std::thread` is spawned
3. The thread fully owns that client socket
4. No shared request queue
5. No blocking central dispatcher

Every client runs independently.

## 🌐 HTTP Example

```cpp
#include "Lumo/Lumo.hpp"

Response test(Request req)
{
    return Response("Hello World!", 200, "OK");
}

int main()
{
    Lumo server("0.0.0.0", 8080);

    server.registerRoute(test, "/test", "GET");

    server.start();
}
```

### Test

```Bash
curl http://localhost:8080/test
```

## JSON Example

```cpp
#include "Lumo/Lumo.hpp"

class User
{
public:
    std::string username;
    std::string email;
    int age;
};
DTO(User, username, email, age);

User globalUser;

Response RegisterUserRoute(Request req)
{
    globalUser = json::parse(req.body).get<User>();
    return Response("User registered successfully", 201, "OK");
}

// 1. Object
Response GetUserRoute(Request req)
{
    return Response(globalUser, 200, "OK");
}

// 2. JSON object
Response GetUserRoute(Request req)
{
    return Response(json(globalUser), 200, "OK");
}

// 3. JSON object in String format
Response GetUserRoute(Request req)
{
    return Response(json(globalUser).dump(), 200, "OK");
}

int main()
{
    Lumo server("0.0.0.0", 8080);

    server.registerRoute(RegisterUserRoute, "/user", "POST");
    server.registerRoute(GetUserRoute, "/user", "GET");

    server.start();
}

```

### Test

#### Register User

```Bash
curl -X POST http://localhost:8080/user -H "Content-Type: application/json" -d '{"username":"john_doe","email":"john@example.com","age":25}'
```

#### Get User

```Bash
curl http://localhost:8080/user
```

## 🔌 Websocket Example

```cpp
#include "Lumo/Lumo.hpp"

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
    server.registerWebSocketRoute(chat, "/chat");
    server.start();
}
```

### Test

```js
const ws = new WebSocket("ws://localhost:8080/chat");

ws.onmessage = (e) => console.log(e.data);

ws.onopen = () => {
  ws.send("Hello!");
};
```

## INSTALL

### build essential for c/c++

```Bash
sudo apt install build-essential
```

### libssl for OpenSSL

```Bash
sudo apt install libssl-dev
```
