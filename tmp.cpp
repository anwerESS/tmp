#include "CurlRequest.h"
#include <stdexcept>

CurlRequest::CurlRequest(const char *url, const char *json_str) {
    curl = curl_easy_init();
    if (curl) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    }
}

CurlRequest::~CurlRequest() {
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void CurlRequest::perform() {
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
}

long CurlRequest::get_response_code() const {
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    return response_code;
}

const std::string& CurlRequest::get_response_body() const {
    return response;
}

size_t CurlRequest::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::string *response = static_cast<std::string *>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}
