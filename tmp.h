#include <iostream>
#include <string>

int main() { 
    char ss[] = "12345678@12_A001@12";
    std::string s1, s2;

    // Convert the C-style string to an std::string
    std::string input(ss);

    // Find the first occurrence of "@12" in input
    size_t pos1 = input.find("@12");

    if (pos1 != std::string::npos) {
        // Extract s1
        s1 = input.substr(0, pos1);

        // Find the second occurrence of "@12" in input
        size_t pos2 = input.find("@12", pos1 + 3);

        if (pos2 != std::string::npos) {
            // Extract s2
            s2 = input.substr(pos1 + 3, pos2 - (pos1 + 3));

            // Print the results
            std::cout << "s1 = \"" << s1 << "\"" << std::endl;
            std::cout << "s2 = \"" << s2 << "\"" << std::endl;
        } else {
            std::cout << "Second '@12' not found in the string." << std::endl;
        }
    } else {
        std::cout << "First '@12' not found in the string." << std::endl;
    }

    return 0;
}
