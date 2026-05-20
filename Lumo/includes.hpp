#pragma once

// Standard library includes
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <csignal>
#include <fstream>

// Networking and system includes
#include <arpa/inet.h>
#include <curl/curl.h>

// OpenSSL includes
#include <openssl/ssl.h>

// JSON library
#include "nlohmann/json.hpp"

// fetch functionnctions
#include "fetch/fetch.hpp"

// timeout for WebSocket message handling
#define MSG_TIMEOUT 100

extern std::atomic<bool> running;

class WS;
class WebSocketManager;
class RequestManager;
struct Request;
class Response;

using WebSocket = std::shared_ptr<WS>;
