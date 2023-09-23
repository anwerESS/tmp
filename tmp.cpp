#include <iostream>
#include <string>

// Function to extract "access_token" from JSON string
std::string extractAccessToken(const std::string& jsonString) {
    std::string tokenKey = "\"access_token\":\"";
    size_t pos = jsonString.find(tokenKey);
    if (pos == std::string::npos) {
        std::cerr << "JSON does not contain 'access_token'." << std::endl;
        return "";
    }

    pos += tokenKey.length();

    size_t endPos = jsonString.find("\"", pos);
    if (endPos == std::string::npos) {
        std::cerr << "Invalid JSON format." << std::endl;
        return "";
    }

    return jsonString.substr(pos, endPos - pos);
}

int main() {
    // Your JSON string
    std::string jsonString = "{\"access_token\":\"ngjdklmjdnmjmkdhyîoqhdfsuihfu^qsidhfi^qfhqdŝi^fsiqhqpihqpdfsihfqipsfhudsqipyvuigfqfdyhidqfgdgsdfshghuiyygfdsg\",\"scope\":\"fdmhiulhlhfdlxhdfxhlloigpopgifdspgsf\",\"token_scope\":\"bearer\",\"expries_in\":599}";


jsonString = "\"access_token";

    std::string accessToken = extractAccessToken(jsonString);

    if (!accessToken.empty()) {
        std::cout << "Access Token: " << accessToken << std::endl;
    } else {
        std::cerr << "Failed to extract access token." << std::endl;
    }

    return 0;
}
