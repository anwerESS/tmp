#include <iostream>
#include <string>
#include <boost/asio.hpp>

int main() {
    // Create an asio io_context
    boost::asio::io_context io_context;

    // Create a resolver to resolve the server hostname and port
    boost::asio::ip::tcp::resolver resolver(io_context);

    // Resolve the server hostname and port
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("sgconnect-hom.fr.world.socgen", "https");

    // Create a socket
    boost::asio::ip::tcp::socket socket(io_context);

    // Connect to the server
    boost::asio::connect(socket, endpoints);

    // Send the HTTP POST request
    std::string request =
        "POST /sgconnect/oauth2/access_token HTTP/1.1\r\n"
        "Host: sgconnect-hom.fr.world.socgen\r\n"
        "Authorization: Basic YWI4MzQwMmEtMmRiMi00NmExLTkyZWMtYzVjODg1NmMzYTlmOjJiMDZoNG45ZGU5OGpjbWZobDFuY2g3a2I1MDJk\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 52\r\n"
        "Connection: close\r\n\r\n"
        "grant_type=client_credentials&scope=api.sgss-institutional-custody-securities-account.v1%20api.sgss-institutional-custody-securities-account.securities-accounts.read";

    boost::asio::write(socket, boost::asio::buffer(request));

    // Read and print the response
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;

    unsigned int status_code;
    response_stream >> status_code;

    std::string status_message;
    std::getline(response_stream, status_message);

    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        std::cout << "Invalid response\n";
        return 1;
    }

    std::string header;
    while (std::getline(response_stream, header) && header != "\r") {
        // Process the response headers if needed
    }

    // Read the response body
    std::stringstream response_body;
    if (response.size() > 0) {
        response_body << &response;
    }
    std::cout << "Response: " << response_body.str() << std::endl;

    // Close the socket
    socket.close();

    return 0;
}


















curl --request POST \
  --url https://sgconnect-hom.fr.world.socgen/sgconnect/oauth2/access_token \
  --header 'Authorization: Basic YWI4MzQwMmEtMmRiMi00NmExLTkyZWMtYzVjODg1NmMzYTlmOjJiMDZoNG45ZGU5OGpjbWZobDFuY2g3a2I1MDJk' \
  --header 'Content-Type: application/x-www-form-urlencoded' \
  --data grant_type=client_credentials \
  --data 'scope=api.sgss-institutional-custody-securities-account.v1 api.sgss-institutional-custody-securities-account.securities-accounts.read'




















CURL *hnd = curl_easy_init();

curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
curl_easy_setopt(hnd, CURLOPT_URL, "https://sgss-inst-cust-securities-acct-uat-api.dev.socgen/api/v1/custody-securities-accounts/do-get-linked-liaison-and-securities-accounts");

struct curl_slist *headers = NULL;
headers = curl_slist_append(headers, "Content-Type: application/json");
headers = curl_slist_append(headers, "Authorization: Bearer eyJ0eXAiOiJKV1QiLCJraWQiOiI5TjdPNm5QRDNGd09tOU9UK1Foem5Bd3lENEE9IiwiYWxnIjoiUlMyNTYifQ.eyJzdWIiOiJhYjgzNDAyYS0yZGIyLTQ2YTEtOTJlYy1jNWM4ODU2YzNhOWYiLCJjdHMiOiJPQVVUSDJfU1RBVEVMRVNTX0dSQU5UIiwiYXVkaXRUcmFja2luZ0lkIjoiZGMxMDNlYjItODkxMy00YjY5LWE0ZGEtZGZiYWUzNTMyMmQzLTMwNDY0MzA2IiwiaXNzIjoiaHR0cHM6Ly9zZ2Nvbm5lY3QtaG9tLmZyLndvcmxkLnNvY2dlbjo0NDMvc2djb25uZWN0L29hdXRoMiIsInRva2VuTmFtZSI6ImFjY2Vzc190b2tlbiIsInRva2VuX3R5cGUiOiJCZWFyZXIiLCJhdXRoR3JhbnRJZCI6IloxMGZ1SWJ6YWxzY2FNME1lQ0FCSWRCZDJMWSIsImNsaWVudF9pZCI6ImFiODM0MDJhLTJkYjItNDZhMS05MmVjLWM1Yzg4NTZjM2E5ZiIsImF1ZCI6ImFiODM0MDJhLTJkYjItNDZhMS05MmVjLWM1Yzg4NTZjM2E5ZiIsIm5iZiI6MTY4Njc4MDMxNSwiZ3JhbnRfdHlwZSI6ImNsaWVudF9jcmVkZW50aWFscyIsInNjb3BlIjpbInNneEBAb3JpZ2luX25ldHdvcmtAQExBTiIsImFwaS5zZ3NzLWluc3RpdHV0aW9uYWwtY3VzdG9keS1zZWN1cml0aWVzLWFjY291bnQuc2VjdXJpdGllcy1hY2NvdW50cy5yZWFkIiwiYXBpLnNnc3MtaW5zdGl0dXRpb25hbC1jdXN0b2R5LXNlY3VyaXRpZXMtYWNjb3VudC52MSJdLCJhdXRoX3RpbWUiOjE2ODY3ODAzMTUsInJlYWxtIjoiLyIsImV4cCI6MTY4Njc4MDkxNSwiaWF0IjoxNjg2NzgwMzE1LCJleHBpcmVzX2luIjo2MDAsImp0aSI6ImlCbXRDNThuS2d4dmg2Y00yQVdkYW8zQTBiVSJ9.fAuAMZOFRxmLwfRHehwV6ciZ0de2zEC0vBJ9sfjkbWFxC04Hh1A4oYMbOFwDP8l2Gd_JP2FndJrkHCzwsq7EbVSDu0isXXBmcDPPoi6NWQSywc4OKukQrzU0RPRHzEE5_NcXXWZD0ru95Nw80TgzFgFA6vWkMJiZDQCLCQcErNI6FbF6ydwDqKLULCxCBfnXHC8PNIs7jtXU2ptGhfZyTbhIao0PpqrShTqhOzKx_d-p-sv6xqZwGvQSnm8G5BJU32VL9sTk1WdcxfEx2seS3wuL0aLcHlDhZeqFKtcamW0AZc7t-Ga0wCtf8DwB0DviLKjgxUFKqdCSTo2qC7SYiQ");
curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "{\"liaisonAccountNumbers\": [\"0540000080008743\"]}");

CURLcode ret = curl_easy_perform(hnd);











		CURL *hnd = curl_easy_init();

		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(hnd, CURLOPT_URL,std::string(m_sg_ecar_service_url + "/do-get-linked-liaison-and-securities-accounts").c_str());

		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, std::string("Authorization: Bearer " + m_acces_token).c_str());
		curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "{\"liaisonAccountNumbers\": [\"" + sacCode + "\"]}");
		curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, onHttpRequest);
		curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &m_response);
		curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(hnd);











*   Trying ::1...
* TCP_NODELAY set
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to localhost (127.0.0.1) port 3128 (#0)
* Establish HTTP proxy tunnel to sgss-inst-cust-securities-acct-uat-api.dev.socgen:443
> CONNECT sgss-inst-cust-securities-acct-uat-api.dev.socgen:443 HTTP/1.1
Host: sgss-inst-cust-securities-acct-uat-api.dev.socgen:443
Proxy-Connection: Keep-Alive

< HTTP/1.1 200 Connection established
< Server: BaseHTTP/0.6 Python/3.10.5
< Date: Wed, 14 Jun 2023 21:52:35 GMT
< Proxy-Agent: BaseHTTP/0.6 Python/3.10.5
<
* Proxy replied OK to CONNECT request
* schannel: SSL/TLS connection with sgss-inst-cust-securities-acct-uat-api.dev.socgen port 443 (step 1/3)
* schannel: checking server certificate revocation
* schannel: sending initial handshake data: sending 220 bytes...
* schannel: sent initial handshake data: sent 220 bytes
* schannel: SSL/TLS connection with sgss-inst-cust-securities-acct-uat-api.dev.socgen port 443 (step 2/3)
* schannel: failed to receive handshake, need more data
* schannel: SSL/TLS connection with sgss-inst-cust-securities-acct-uat-api.dev.socgen port 443 (step 2/3)
* schannel: encrypted data got 3459
* schannel: encrypted data buffer: offset 3459 length 4096
* schannel: next InitializeSecurityContext failed: Unknown error (0x80092012) - The revocation function was unable to check revocation for the certificate.
* Closing connection 0
* schannel: shutting down SSL/TLS connection with sgss-inst-cust-securities-acct-uat-api.dev.socgen port 443
* schannel: clear security context handle
