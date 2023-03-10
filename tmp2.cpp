#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

std::string base64_encode(const std::string& input) {
    std::string output;

    static const std::string base64_chars =
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    unsigned char const* bytes_to_encode = reinterpret_cast<const unsigned char*>(input.c_str());
    size_t in_len = input.length();

    size_t i = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                output += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (size_t j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (size_t j = 0; j < i + 1; j++)
            output += base64_chars[char_array_4[j]];

        while (i++ < 3)
            output += '=';
    }

    return output;
}

class BasicAuth {
public:
    BasicAuth(const std::string& username, const std::string& password) 
    : username_(username), password_(password) {}

    std::string get_header() const {
        std::stringstream ss;
        ss << username_ << ":" << password_;
        std::string auth_string = ss.str();
        return "Basic " + base64_encode(auth_string);
    }

private:
    std::string username_;
    std::string password_;
};

int main() {
    std::string username = "myusername";
    std::string password = "mypassword";
    BasicAuth auth(username, password);
    std::cout << "Authorization header: " << auth.get_header() << std::endl;
    return 0;
}
