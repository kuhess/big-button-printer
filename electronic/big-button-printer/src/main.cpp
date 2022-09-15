#include <Arduino.h>
#include <SoftwareSerial.h>

#include <AceButton.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <jled.h>
#include <LittleFS.h>

#include "HttpService.h"
#include "TPrinter.h"

#include "wifi.h"

#ifndef WIFI_H
#define WIFI_SSID "<SSID>"
#define WIFI_PASSWORD "<PASSWORD>"
#endif


// Printer
const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;
// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
// /!\ DO NOT USE SERIAL TO LOG, ONLY FOR PRINTER
SoftwareSerial mySerial(rxPin, txPin);
Tprinter printer(&mySerial, printerBaudrate);

// Button
const byte led_pin = D5;
const byte button_pin = D2;
JLed led = JLed(led_pin);
ace_button::AceButton button(button_pin, LOW);

HTTPClient httpClient;
WiFiClientSecure wifiClient;

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

HttpService http_service(&httpClient, &wifiClient);

void onWiFiConnected(const WiFiEventStationModeGotIP &event) {
    led.Breathe(2000).Forever();
}

void onWiFiDisconnected(const WiFiEventStationModeDisconnected &event) {
    led.Blink(250, 250).Forever();
}

void setupPrinter() {
    mySerial.begin(printerBaudrate);
    printer.begin();
    printer.enableDtr(dtrPin, HIGH);
    printer.setCodePage(CODEPAGE_CP437);
    printer.setCharset(CHARSET_USA);
}

void onButtonClicked() {
    String url = "https://raw.githubusercontent.com/kuhess/test-images/main/384.raw";
    if (http_service.DownloadDataFrom(url, "/image.raw")) {
        File image_file = LittleFS.open("/image.raw", "r");
        printer.printRawImage(&image_file, 384);
    }
}

void onButtonDoubleClicked() {
    String url = "https://raw.githubusercontent.com/kuhess/test-images/main/001.txt";
    String text = http_service.GetStringFrom(url);
    if (!text.isEmpty()) {
        printer.println(text);
    }
}

void handleEvent(ace_button::AceButton * /* button */, uint8_t eventType, uint8_t /* buttonState */) {
    switch (eventType) {
        case ace_button::AceButton::kEventClicked:
        case ace_button::AceButton::kEventReleased:
            onButtonClicked();
            break;
        case ace_button::AceButton::kEventDoubleClicked:
            onButtonDoubleClicked();
            break;
    }
}

void setup() {
    // Button & LED
    pinMode(led_pin, OUTPUT);
    pinMode(button_pin, INPUT_PULLUP);

    led.Blink(250, 250).Forever();

    ace_button::ButtonConfig *buttonConfig = button.getButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterDoubleClick);

    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Register event handlers
    stationConnectedHandler = WiFi.onStationModeGotIP(&onWiFiConnected);
    stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onWiFiDisconnected);

    // HTTP
    wifiClient.setInsecure();

    // File System
    LittleFS.begin();

    // Printer
    setupPrinter();
}

void loop() {
    button.check();
    led.Update();
}
