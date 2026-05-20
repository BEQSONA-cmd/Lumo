#pragma once

#include <curl/curl.h>
#include <string>
#include <map>
#include <functional>

struct FetchResult
{
    long status = 0;
    CURLcode curlCode = CURLE_OK;
    std::string body;
};

struct FetchOptions
{
    std::string method = "GET";
    std::map<std::string, std::string> headers;
    std::string body = "";

    std::function<void(const std::string &)> streamCallback = nullptr;
};

FetchResult fetch(const std::string &url, const FetchOptions &options = FetchOptions());
