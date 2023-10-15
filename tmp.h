#include <iostream>
#include <string>
#include <locale>
#include <sstream>

double convertStringToDouble(const std::string& s) {
    // Check if the input string is empty
    if (s.empty()) {
        return 0.0;
    }

    std::string formattedString;

    // Remove spaces
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] != ' ') {
            formattedString += s[i];
        }
    }

    // Replace a comma with a dot for decimal point if needed
    for (size_t i = 0; i < formattedString.length(); ++i) {
        if (formattedString[i] == ',') {
            formattedString[i] = '.';
        }
    }

    // Use a stringstream for conversion
    std::istringstream iss(formattedString);
    double result = 0.0;

    if (!(iss >> result)) {
        // Conversion failed
        throw std::invalid_argument("Invalid input string");
    }

    return result;
}
