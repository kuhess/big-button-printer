#ifndef BIG_BUTTON_PRINTER_HTTPSERVICE_H
#define BIG_BUTTON_PRINTER_HTTPSERVICE_H

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>


class HttpService {
public:
    explicit HttpService(HTTPClient *httpClient, WiFiClientSecure *wifiClient);

    String GetStringFrom(String url);

    String GetStringFromWithRetry(String url, size_t maxRetry = 3);

    bool DownloadDataFrom(String url, String filename);

    bool DownloadDataFromWithRetry(String url, String filename, size_t maxRetry = 3);

private:
    HTTPClient *httpClient_;
    WiFiClientSecure *wifiClient_;
};


#endif //BIG_BUTTON_PRINTER_HTTPSERVICE_H
