
#ifndef CURL_REQUEST_H
#define CURL_REQUEST_H

#include <string>
#include <curl/curl.h>

class CurlRequest {
public:
    CurlRequest(const char *url, const char *json_str);
    ~CurlRequest();

    void perform();

    long get_response_code() const;
    const std::string& get_response_body() const;

private:
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string response;

    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
};

#endif
