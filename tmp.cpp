std::string extractBankCode(const std::string &json) {
    std::string bankCode;
    size_t pos = json.find("\"bankCode\"");
    if (pos != std::string::npos) {
        // Find the colon after "bankCode":
        size_t colonPos = json.find(":", pos);
        if (colonPos != std::string::npos) {
            // Find the opening double quote after the colon:
            size_t start = json.find("\"", colonPos);
            if (start != std::string::npos) {
                // Find the closing double quote after "bankCode" value:
                size_t end = json.find("\"", start + 1);
                if (end != std::string::npos) {
                    // Extract the bankCode value:
                    bankCode = json.substr(start + 1, end - start - 1);
                }
            }
        }
    }
    return bankCode;
}
