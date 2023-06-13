#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

std::string trim(const std::string& str)
{
    std::string trimmed = str;
    std::string::size_type pos = trimmed.find_first_not_of(' ');
    if (pos != std::string::npos)
        trimmed.erase(0, pos);
    pos = trimmed.find_last_not_of(' ');
    if (pos != std::string::npos)
        trimmed.erase(pos + 1);
    return trimmed;
}

std::string extractToken(const std::string& jsonString)
{
    std::string token;
    std::string::size_type startPos = jsonString.find("\"access_token\"");

    if (startPos != std::string::npos)
    {
        startPos = jsonString.find(':', startPos);
        if (startPos != std::string::npos)
        {
            startPos = jsonString.find('"', startPos);
            if (startPos != std::string::npos)
            {
                std::string::size_type endPos = jsonString.find('"', startPos + 1);
                if (endPos != std::string::npos)
                {
                    token = jsonString.substr(startPos + 1, endPos - startPos - 1);
                    token = trim(token);
                }
            }
        }
    }

    return token;
}

int main()
{
    std::string jsonString = "{\"access_token\" : \"eyJ0eXAiOiJKV1QiLCJraWQiOiI5TjdPNm5QRDNGd09tOU9UK1Foem5Bd3lENEE9IiwiYWxnIjoiUlMyNTYifQ.eyJzdWIiOiJhYjgzNDAyYS0yZGIyLTQ2YTEtOTJlYy1jNWM4ODU2YzNhOWYiLCJjdHMiOiJPQVVUSDJfU1RBVEVMRVNTX0dSQU5UIiwiYXVkaXRUcmFja2luZ0lkIjoiNDMzOWQ0NDItYjA1Ni00YmQ4LWFjYWYtZWUyNmE3OGRlMGUyLTI2NzU4MTY3MSIsImlzcyI6Imh0dHBzOi8vc2djb25uZWN0LWhvbS5mci53b3JsZC5zb2NnZW46NDQzL3NnY29ubmVjdC9vYXV0aDIiLCJ0b2tlbk5hbWUiOiJhY2Nlc3NfdG9rZW4iLCJ0b2tlbl90eXBlIjoiQmVhcmVyIiwiYXV0aEdyYW50SWQiOiJ0TEc1OUEwMm9XYV9EWFlNMjFMVlVrek1hWUUiLCJjbGllbnRfaWQiOiJhYjgzNDAyYS0yZGIyLTQ2YTEtOTJlYy1jNWM4ODU2YzNhOWYiLCJhdWQiOiJhYjgzNDAyYS0yZGIyLTQ2YTEtOTJlYy1jNWM4ODU2YzNhOWYiLCJuYmYiOjE2ODY2OTE0OTYsImdyYW50X3R5cGUiOiJjbGllbnRfY3JlZGVudGlhbHMiLCJzY29wZSI6WyJzZ3hAQG9yaWdpbl9uZXR3b3JrQEBMQU4iLCJhcGkuc2dzcy1pbnN0aXR1dGlvbmFsLWN1c3RvZHktc2VjdXJpdGllcy1hY2NvdW50LnNlY3VyaXRpZXMtYWNjb3VudHMucmVhZCIsImFwaS5zZ3NzLWluc3RpdHV0aW9uYWwtY3VzdG9keS1zZWN1cml0aWVzLWFjY291bnQudjEiXSwiYXV0aF90aW1lIjoxNjg2NjkxNDk2LCJyZWFsbSI6Ii8iLCJleHAiOjE2ODY2OTIwOTYsImlhdCI6MTY4NjY5MTQ5NiwiZXhwaXJlc19pbiI6NjAwLCJqdGkiOiJoREVNZmkxQl84VWpaYkFjNlJ6b3pCdlN6WDQifQ.jjdfEGaLFRjnE0dX51iRH4QsbDOLv-tQxfcafSvsW-g8n6JNm5sdEHMRzDI3pra_ipzSMp0KtFALY7fik7fSKpR6yfiSI4l0gXnMpKeL06JCQr2ru3J271W2oPJ3LRPhoxZrrGuuPo7IM98LBoj_hSESlJ3PzQYeBeGq-kPtHK9JHNp4F5JWdoB4p9iEg2yJj-VmruYpD44wnuzR9Fun_69To2OY0iUUe2mKykktq8DiCPZpwOzjY3lSvKf0HKFuzwqMVAxvG531zlRhdk8Rg17xPkiQuuDE_DATWHsqgG2b3MQTYL2hYKyMA6m7zS9-FD8HmoQoO_Hskb1HSoB-FA\",\"scope\":\"sgx@@origin_network@@LAN api.sgss-institutional-custody-securities-account.securities-accounts.read api.sgss-institutional-custody-securities-account.v1\",\"token_type\":\"Bearer\",\"expires_in\":599}";

    std::string token = extractToken(jsonString);

    std::cout << "Token: " << token << std::endl;

    return 0;
}
