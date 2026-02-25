# Lumo

A lightweight, multithreaded HTTP & WebSocket engine built in modern C++.

No runtime bloat.  
No event loop overhead.  
No heavy dependencies.

Just raw TCP power.

---

## Why Lumo?

- 🔥 Multithreaded per-connection model
- 🧵 No request queue bottlenecks
- ⚡ Zero runtime framework overhead
- 💾 Minimal RAM usage
- 📦 Tiny footprint
- 🌐 Native HTTP + WebSocket support
- 🔒 Thread-safe architecture

Designed for performance-first backend systems.

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

This eliminates:

- ❌ Global bottlenecks
- ❌ Request waiting queues
- ❌ Event-loop congestion

Every client runs independently.

## INSTALL

### build essential for c/c++
```Bash
sudo apt install build-essential
```

### libssl for OpenSSL
```Bash
sudo apt install libssl-dev
```

## 🌐 HTTP Example

```cpp
#include "Lumo/Lumo.hpp"

Response hello(Request req)
{
    return Response("Hello World!", 200, "OK");
}

int main()
{
    Lumo server("0.0.0.0", 8080);
    server.registerRoute(hello, "/hello", "GET");
    server.start();
}
```

## 🔌 Websocket Example

```cpp
#include "Lumo/Lumo.hpp"

void chat(std::shared_ptr<WebSocket> ws)
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
