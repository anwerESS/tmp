    public static boolean isValidESMString(String input) {
        // Check if input is null or empty
        if (input == null || input.isEmpty()) {
            return false;
        }

        // Define regex pattern:
        // ^ESM        - Must start with "ESM"
        // [a-zA-Z0-9] - Only alphanumeric chars allowed
        // {0,13}      - Between 0 and 13 chars after "ESM" (total max length = 16)
        // $           - End of string
        String regex = "^ESM[a-zA-Z0-9]{0,13}$";

        // Check if the input matches the regex
        return input.matches(regex);
    }
