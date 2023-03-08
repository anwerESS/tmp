#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>

class CurlRequest {
public:
    CurlRequest(const char *url, const char *json_str) {
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

    ~CurlRequest() {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    void perform() {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }
    }

    long get_response_code() const {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        return response_code;
    }

    const std::string& get_response_body() const {
        return response;
    }

private:
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string response;

    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
        std::string *response = static_cast<std::string *>(userdata);
        response->append(ptr, size * nmemb);
        return size * nmemb;
    }
};

int main() {
    const char *url = "https://example.com/api";
    const char *json_str = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";

    try {
        CurlRequest request(url, json_str);
        request.perform();

        std::cout << "Response code: " << request.get_response_code() << std::endl;
        std::cout << "Response body: " << request.get_response_body() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "CurlRequest failed: " << e.what() << std::endl;
    }

    return 0;
}
