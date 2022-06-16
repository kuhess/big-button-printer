#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecureBearSSL.h>

#include <AceButton.h>
#include <ArduinoJson.h>
#include <TPrinter.h>

#include "rocky.h"
#include "wifi.h"

#ifndef WIFI_H
#define WIFI_SSID "<SSID>"
#define WIFI_PASSWORD "<PASSWORD>"
#endif

const int BUTTON_PIN = D2;
const int LED_PIN = D5;

ace_button::AceButton button(BUTTON_PIN, LOW);

const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;

// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
SoftwareSerial mySerial(rxPin, txPin);
Tprinter myPrinter(&mySerial, printerBaudrate);

HTTPClient httpClient;

const char cowAscii[] = "                __n__n__\n"
                        "         .------`-\\00/-'\n"
                        "        /  ##  ## (oo)\n"
                        "       / \\## __   ./\n"
                        "          |//YY \\|/\n"
                        "          |||   |||";

void printRocky() {
    digitalWrite(LED_BUILTIN, HIGH);

    myPrinter.justify('C');

    myPrinter.printBitmap(rocky, rockyWidth, rockyHeight, 1);
    myPrinter.feed(1);
    myPrinter.println("Rocky");
    myPrinter.feed(3);

    digitalWrite(LED_BUILTIN, LOW);
}

void printCow() {
    myPrinter.justify('L');
    myPrinter.println(cowAscii);
    myPrinter.feed(3);
}

String getAscii() {
    String text;

    std::unique_ptr<BearSSL::WiFiClientSecure> wifiClient(new BearSSL::WiFiClientSecure);
    wifiClient->setInsecure();

    bool begin_success = httpClient.begin(*wifiClient,
                                          "https://raw.githubusercontent.com/kuhess/test-images/main/001.txt");

    if (!begin_success) {
        // Serial.printf("[HTTP} Unable to connect\n");
        return "";
    }

    int httpCode = httpClient.GET();

    if (httpCode < 0) {
        // Serial.printf("[HTTP] GET failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
        return "";
    }

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        text = httpClient.getString();
    }

    httpClient.end();

    return text;
}

void printAscii() {
    String ascii;

    size_t retry = 0;

    while (ascii.isEmpty() && retry < 3) {
        ascii = getAscii();
        retry++;
    }

    if (!ascii.isEmpty()) {
        myPrinter.println(ascii.c_str());
        myPrinter.feed(3);
    } else {
        myPrinter.printf("error retry=%d\n", retry);
    }
}

String getDatetime() {
    String text;
    WiFiClient wifiClient;

    bool begin_success = httpClient.begin(wifiClient, "http://worldtimeapi.org/api/timezone/Europe/Paris");

    if (!begin_success) {
        // Serial.printf("[HTTP} Unable to connect\n");
        return "";
    }

    int httpCode = httpClient.GET();

    if (httpCode < 0) {
        // Serial.printf("[HTTP] GET failed, error: %s\n", httpClient.errorToString(httpCode).c_str());
        return "";
    }

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, httpClient.getStream());

        String datetime = doc["datetime"].as<String>();
        text = datetime.substring(0, 19);
    }

    httpClient.end();

    return text;
}

void printDatetime() {
    String datetime;
    String output;

    size_t retry = 0;

    while (datetime.isEmpty() && retry < 3) {
        datetime = getDatetime();
        retry++;
    }

    if (!datetime.isEmpty()) {
        output = datetime;
    }

    myPrinter.println(output.c_str());
    myPrinter.feed(2);
}

void handleEvent(ace_button::AceButton *button, uint8_t eventType, uint8_t buttonState) {
    switch (eventType) {
        case ace_button::AceButton::kEventClicked:
        case ace_button::AceButton::kEventReleased:
            digitalWrite(LED_PIN, HIGH);
            printAscii();
            break;
        case ace_button::AceButton::kEventDoubleClicked:
            digitalWrite(LED_PIN, LOW);
            printDatetime();
            break;
    }
}

void setup() {
    // WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (!WiFi.isConnected()) {
        delay(100);
    }
    // Setup HTTP client
    httpClient.useHTTP10(true);
    httpClient.setReuse(false);

    // Printer
    mySerial.begin(printerBaudrate);
    myPrinter.begin();
    myPrinter.setCodePage(CODEPAGE_CP437);
    myPrinter.setCharset(CHARSET_USA);
    myPrinter.enableDtr(dtrPin, HIGH);

    // Button & LED
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    ace_button::ButtonConfig *buttonConfig = button.getButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterDoubleClick);
}

void loop() {
    button.check();
}
