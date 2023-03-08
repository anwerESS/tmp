#include <iostream>
#include <curl/curl.h>

int main() {
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/api");

        // Set the JSON payload
        const char *json_str = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);

        // Set the headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response callback
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
            std::string *response = static_cast<std::string *>(userdata);
            response->append(ptr, size * nmemb);
            return size * nmemb;
        });
        std::string response;

        // Perform the request
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            std::cout << "Response code: " << response_code << std::endl;
            std::cout << "Response body: " << response << std::endl;
        } else {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return 0;
}
