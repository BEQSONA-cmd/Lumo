#include "fetch.hpp"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total = size * nmemb;

    std::string *output =
        static_cast<std::string *>(userp);

    output->append(
        static_cast<char *>(contents),
        total);

    return total;
}

size_t StreamCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t total = size * nmemb;

    auto* handler = static_cast<std::function<void(const std::string &)> *>(userp);

    std::string chunk(static_cast<char*>(contents), total);

    if (handler && *handler)
        (*handler)(chunk);

    return total;
}


FetchResult fetch(const std::string &url, const FetchOptions &options)
{
CURL *curl = curl_easy_init();

    FetchResult result;

    if (!curl)
    {
        result.curlCode = CURLE_FAILED_INIT;
        return result;
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // METHOD
    if (options.method == "POST")
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
    else if (options.method != "GET")
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, options.method.c_str());

    // BODY
    if (!options.body.empty())
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, options.body.size());
    }

    // HEADERS
    struct curl_slist *chunk = nullptr;

    for (const auto &[key, value] : options.headers)
    {
        std::string header = key + ": " + value;
        chunk = curl_slist_append(chunk, header.c_str());
    }

    if (chunk)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    // SSL
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // TIMEOUTS
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    // REDIRECTS
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // CALLBACKS
    if (options.streamCallback)
    {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &options.streamCallback);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }

    // PERFORM REQUEST
    result.curlCode = curl_easy_perform(curl);

    // STATUS
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);

    result.body = std::move(response);

    // CLEANUP
    if (chunk)
        curl_slist_free_all(chunk);

    curl_easy_cleanup(curl);

    return result;
}
