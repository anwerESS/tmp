std::string padStringToDesiredLength(const std::string& s1, const std::string& s2, int desiredLength) {
    // Calculate the number of characters needed to pad to reach the desired length
    int paddingSize = desiredLength - s1.length() - s2.length();

    if (paddingSize < 0) {
        // Handle the case where the combined length is already longer than the desired length
        std::cerr << "Error: Combined length exceeds the desired length." << std::endl;
        return "";
    }

    // Construct the padded string
    return s1 + std::string(paddingSize, ' ') + s2;
}
