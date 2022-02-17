#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>
#include <TPrinter.h>

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
Tprinter myPrinter(&mySerial, printerBaudrate);

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(!WiFi.isConnected()) {
    delay(100);
  }
}

void setupPrinter() {
  mySerial.begin(printerBaudrate);
  myPrinter.begin();
  myPrinter.enableDtr(dtrPin, HIGH);
}

void setup() {
  Serial.begin(115200);

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

  WiFiClient wifiClient;

  bool begin_success = httpClient.begin(wifiClient, "http://worldtimeapi.org/api/timezone/Europe/Paris");
  
  if (!begin_success) {
    Serial.printf("[HTTP} Unable to connect\n");
    return;
  }

  int httpCode = httpClient.GET();

  if (httpCode < 0) {
    Serial.printf("[HTTP] GET failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
    return;
  }

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, httpClient.getStream());
    
    String datetime = doc["datetime"].as<String>();
    String text = datetime.substring(0, 19);

    myPrinter.println(text.c_str());
    myPrinter.feed(2);

    delay(1000);
  }

  httpClient.end();
}
