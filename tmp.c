#include <iostream>
#include <string>

bool isDecimal(const std::string& str) {
    bool hasDot = false;

    // Check each character in the string
    for (char c : str) {
        if (c == '.') {
            // Ensure only one dot is present
            if (hasDot) {
                return false; // More than one dot found
            }
            hasDot = true;
        } else if (!std::isdigit(c)) {
            return false; // Non-digit character found
        }
    }

    // Ensure at least one digit before and after the dot
    return hasDot && str.find('.') != 0 && str.find('.') != str.length() - 1;
}

int main() {
    std::string str = "123.45";

    if (isDecimal(str)) {
        std::cout << "'" << str << "' is in the '<number>.<number>' format." << std::endl;
    } else {
        std::cout << "'" << str << "' is not in the '<number>.<number>' format." << std::endl;
    }

    return 0;
}
