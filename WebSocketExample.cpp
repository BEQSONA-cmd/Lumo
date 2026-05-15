#include "Lumo/Lumo.hpp"

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
