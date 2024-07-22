#include <iostream>
#include <string>
#include <vector>

// Function to encode a single character to Base64
char base64EncodeChar(unsigned char c) {
    if (c < 26) return 'A' + c;
    if (c < 52) return 'a' + (c - 26);
    if (c < 62) return '0' + (c - 52);
    if (c == 62) return '+';
    if (c == 63) return '/';
    return '=';
}

// Function to convert a UTF-8 string to Base64
std::string utf8ToBase64(const std::string& input) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(input.c_str());
    size_t len = input.length();
    std::string output;

    for (size_t i = 0; i < len; i += 3) {
        // Pack three bytes into a 24-bit number
        unsigned int value = 0;
        value |= (bytes[i] << 16);
        if (i + 1 < len) {
            value |= (bytes[i + 1] << 8);
        }
        if (i + 2 < len) {
            value |= bytes[i + 2];
        }

        // Encode the 24-bit number as four 6-bit Base64 characters
        output.push_back(base64EncodeChar((value >> 18) & 0x3F));
        output.push_back(base64EncodeChar((value >> 12) & 0x3F));
        if (i + 1 < len) {
            output.push_back(base64EncodeChar((value >> 6) & 0x3F));
        } else {
            output.push_back('=');
        }
        if (i + 2 < len) {
            output.push_back(base64EncodeChar(value & 0x3F));
        } else {
            output.push_back('=');
        }
    }

    return output;
}

int main() {
    std::string utf8String = "Hello, World!";
    std::string base64String = utf8ToBase64(utf8String);

    std::cout << "UTF-8 String: " << utf8String << std::endl;
    std::cout << "Base64 Encoded: " << base64String << std::endl;

    return 0;
}
