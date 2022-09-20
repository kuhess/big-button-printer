#include <Arduino.h>
#include <SoftwareSerial.h>

#include <AceButton.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266TrueRandom.h>
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
    led.Blink(200, 200).Forever();
}

void setupPrinter() {
    mySerial.begin(printerBaudrate);
    printer.begin();
    printer.enableDtr(dtrPin, HIGH);
    printer.setCodePage(CODEPAGE_CP437);
    printer.setCharset(CHARSET_USA);
}

void onButtonClicked() {
    if (!WiFi.isConnected()) {
        return;
    }

    uint32_t number = ESP8266TrueRandom.random(1000);

    printer.feed(2);
    printer.justify('C');
    printer.print("< Jeu n\xf8");
    printer.print(number);
    printer.println(" >");
    printer.feed(1);

    char filename[16];
    sprintf(filename, "game_%04d.raw", number);
    String url = "https://raw.githubusercontent.com/kuhess/printer-images/main/" + String(filename);
    if (http_service.DownloadDataFrom(url, "/image.raw")) {
        File image_file = LittleFS.open("/image.raw", "r");
        printer.printRawImage(&image_file, 384);
    } else {
        printer.println("Whoops...");
        printer.feed(1);
        printer.justify('L');
        printer.println("                __n__n__\n"
            "         .------`-\\00/-'\n"
            "        /  ##  ## (oo)\n"
            "       / \\## __   ./\n"
            "          |//YY \\|/\n"
            "          |||   |||");
        printer.justify('C');
        printer.feed(1);
        printer.println("Un probl\x8a" "me est survenu");
    }
    printer.feed(6);
}

void onButtonDoubleClicked() {
    File logo = LittleFS.open("/logo.raw", "r");

    printer.feed(2);
    printer.justify('C');
    printer.println("< La Bo\x8c" "te \x85" " Tickets >");
    printer.feed(1);
    printer.printRawImage(&logo, 384);
    printer.feed(1);
    printer.println("hello\x40quentinsuire.com");
    printer.feed(1);
    printer.setMode(FONT_B);
    printer.println("projet r\x82" "alis\x82" " en 2022");
    printer.unsetMode(FONT_B);
    printer.feed(5);
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
