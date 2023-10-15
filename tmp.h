std::string formatFloat(double d) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << d;
    std::string s = stream.str();
    std::string integerPart = s.substr(0, s.find('.'));
    std::string decimalPart = s.substr(s.find('.') + 1);
    std::string formattedIntegerPart;
    int count = 0;
    for (int i = integerPart.size() - 1; i >= 0; i--) {
        formattedIntegerPart = integerPart[i] + formattedIntegerPart;
        count++;
        if (count == 3 && i != 0) {
            formattedIntegerPart = " " + formattedIntegerPart;
            count = 0;
        }
    }
    return formattedIntegerPart + "," + decimalPart;
}
