
#include "HttpService.h"

#include <WiFiClientSecureBearSSL.h>
#include <LittleFS.h>


HttpService::HttpService(HTTPClient *httpClient, WiFiClientSecure *wifiClient) : httpClient_(httpClient),
                                                                                 wifiClient_(wifiClient) {}


String HttpService::GetStringFrom(String url) {
    String text;

    bool begin_success = httpClient_->begin(*wifiClient_, url);
    if (!begin_success) {
        // Unable to connect
        return "";
    }

    int httpCode = httpClient_->GET();
    if (httpCode < 0) {
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


bool HttpService::DownloadDataFrom(String url, String filename) {
    httpClient_->begin(*wifiClient_, url);

    // Start connection and send HTTP header
    int httpCode = httpClient_->GET();
    if (httpCode > 0) {
        fs::File f = LittleFS.open(filename, "w+");
        if (!f) {
            return false;
        }
        // HTTP header has been send and Server response header has been handled

        // File found at server
        if (httpCode == HTTP_CODE_OK) {

            // Get length of document (is -1 when Server sends no Content-Length header)
            int total = httpClient_->getSize();
            int len = total;

            // Create buffer for read
            uint8_t buff[128] = { 0 };

            // Get tcp stream
            WiFiClient * stream = httpClient_->getStreamPtr();

            // Read all data from server
            while (httpClient_->connected() && (len > 0 || len == -1)) {
                // Get available data size
                size_t size = stream->available();

                if (size) {
                    // Read up to 128 bytes
                    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                    // Write it to file
                    f.write(buff, c);

                    // Calculate remaining bytes
                    if (len > 0) {
                        len -= c;
                    }
                }
                yield();
            }
        }
        f.close();
    } else {
        return false;
    }
    httpClient_->end();
    return true; // File was fetched from web
}


bool HttpService::DownloadDataFromWithRetry(String url, String filename, size_t maxRetry) {
    bool is_success = false;
    size_t retry = 0;

    while (!is_success && retry < maxRetry) {
        is_success = DownloadDataFrom(url, filename);
        retry++;
    }

    return is_success;
}
