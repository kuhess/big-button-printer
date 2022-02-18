#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecureBearSSL.h>

// Include LittleFS
#include <FS.h>
#include "LittleFS.h"

#include <ArduinoJson.h>
#include <Adafruit_Thermal.h>

#include "wifi.h"

#ifndef WIFI_H
#define WIFI_SSID "<SSID>"
#define WIFI_PASSWORD "<PASSWORD>"
#endif

HTTPClient httpClient;

const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;
// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
SoftwareSerial mySerial(rxPin, txPin);
Adafruit_Thermal myPrinter(&mySerial, dtrPin);

bool downloadFile(String url, String filename) {

  // If it exists then no need to fetch it
  if (LittleFS.exists(filename) == true) {
    Serial.println("Found " + filename);
    return 0;
  }

  Serial.println("Downloading "  + filename + " from " + url);

  // Check WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    Serial.print("[HTTP] begin...\n");

    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    httpClient.begin(*client, url);

    Serial.print("[HTTP] GET...\n");
    // Start connection and send HTTP header
    int httpCode = httpClient.GET();
    if (httpCode > 0) {
      fs::File f = LittleFS.open(filename, "w+");
      if (!f) {
        Serial.println("file open failed");
        return 0;
      }
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // File found at server
      if (httpCode == HTTP_CODE_OK) {

        // Get length of document (is -1 when Server sends no Content-Length header)
        int total = httpClient.getSize();
        int len = total;

        // Create buffer for read
        uint8_t buff[128] = { 0 };

        // Get tcp stream
        WiFiClient * stream = httpClient.getStreamPtr();

        // Read all data from server
        while (httpClient.connected() && (len > 0 || len == -1)) {
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
        Serial.println();
        Serial.print("[HTTP] connection closed or file end.\n");
      }
      f.close();
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
    }
    httpClient.end();
  }
  return 1; // File was fetched from web
}

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(!WiFi.isConnected()) {
    delay(100);
  }
}

void setupPrinter() {
  mySerial.begin(printerBaudrate);
  myPrinter.begin();
}

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(3000);

  // Initialise LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  setupWiFi();
  setupPrinter();

  // Setup HTTP client
  httpClient.useHTTP10(true);
  httpClient.setReuse(false);
}

void loop() {
  if (!WiFi.isConnected()) {
    delay(5000);
  }

  String url = "https://raw.githubusercontent.com/kuhess/test-images/main/rocky_bw.raw";
  bool loaded_ok = downloadFile(url, "/local_image.raw");

  File local_file = LittleFS.open("/local_image.raw", "r");
  myPrinter.printBitmap(384, 384, (Stream*) &local_file);
  myPrinter.feed(2);
  
  delay(30000);
}
