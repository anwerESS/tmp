std::string extractBankCode(const std::string& json) {
    // Find the start of "securitiesAccountBasicData" object
    size_t basicDataStart = json.find("\"securitiesAccountBasicData\": {");
    if (basicDataStart == std::string::npos) {
        return ""; // "securitiesAccountBasicData" object not found
    }

    // Find the start of "bankCode" key within "securitiesAccountBasicData"
    size_t bankCodeStart = json.find("\"bankCode\": \"", basicDataStart);
    if (bankCodeStart == std::string::npos) {
        return ""; // "bankCode" key not found within "securitiesAccountBasicData"
    }

    // Find the end of "bankCode" value
    size_t bankCodeEnd = json.find("\"", bankCodeStart + 14);
    if (bankCodeEnd == std::string::npos) {
        return ""; // Unclosed "bankCode" value
    }

    // Extract and return the "bankCode" value
    return json.substr(bankCodeStart + 14, bankCodeEnd - bankCodeStart - 14);
}
 
