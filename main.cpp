#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>


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
