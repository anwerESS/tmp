

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
