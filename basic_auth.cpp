
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <openssl/bio.h>
#include <openssl/evp.h>

std::string base64_encode(const std::string& input) {
    std::string output;
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO *bmem = BIO_new(BIO_s_mem());
    BIO_push(b64, bmem);
    BIO_write(b64, input.c_str(), input.length());
    BIO_flush(b64);
    char *buffer = nullptr;
    size_t length = BIO_get_mem_data(bmem, &buffer);
    if (length > 0) {
        output.assign(buffer, length-1);
    }
    BIO_free_all(b64);
    return output;
}

std::string encode_basic_auth(const std::string& username, const std::string& password) {
    std::stringstream ss;
    ss << username << ":" << password;
    std::string auth_string = ss.str();
    return "Basic " + base64_encode(auth_string);
}

int main() {
    std::string username = "myusername";
    std::string password = "mypassword";
    std::string auth_header = encode_basic_auth(username, password);
    std::cout << "Authorization header: " << auth_header << std::endl;
    return 0;
}
