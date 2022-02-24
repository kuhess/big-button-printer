#include "HttpService.h"

#include <WiFiClientSecureBearSSL.h>

HttpService::HttpService(HTTPClient *httpClient, WiFiClientSecure &wifiClient) : httpClient_(httpClient),
                                                                                 wifiClient_(wifiClient) {}

String HttpService::GetStringFrom(String url) {
    String text;

    bool begin_success = httpClient_->begin(wifiClient_, url);
    if (!begin_success) {
        // Unable to connect
        return "";
    }

    int httpCode = httpClient_->GET();
    if (httpCode < 0) {
        // error on GET: Serial.println(httpClient_->errorToString(httpCode).c_str());
        return "";
    }
    if (httpCode != HTTP_CODE_OK) {
        // not a HTTP 200
        return "";
    }

    text = httpClient_->getString();

    httpClient_->end();

    return text;
}


String HttpService::GetStringFromWithRetry(String url, size_t maxRetry) {
    String text;

    size_t retry = 0;

    while (text.isEmpty() && retry < maxRetry) {
        text = GetStringFrom(url);
        retry++;
    }

    return text;
}
