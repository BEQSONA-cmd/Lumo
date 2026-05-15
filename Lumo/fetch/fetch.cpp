#include "fetch.hpp"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    if (!output)
        return 0;

    size_t total = size * nmemb;
    output->append(static_cast<char *>(contents), total);
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
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, options.method.c_str());

    // timeouts
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

    if (!options.body.empty())
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str());

    struct curl_slist *chunk = nullptr;

    for (const auto &[key, value] : options.headers)
    {
        std::string header = key + ": " + value;
        chunk = curl_slist_append(chunk, header.c_str());
    }

    if (chunk)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    result.curlCode = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result.status);

    result.body = std::move(response);

    if (chunk)
        curl_slist_free_all(chunk);

    curl_easy_cleanup(curl);

    return result;
}
