#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#include <AceButton.h>
#include <jled.h>

#include "HttpService.h"
#include "LedManager.h"
#include "PrintService.h"
#include "rocky.h"
#include "wifi.h"

#ifndef WIFI_H
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"
#endif

const uint8_t PRINTER_DTR_PIN = D1;
const uint8_t PRINTER_RX_PIN = D9;
const uint8_t PRINTER_TX_PIN = D10;
const uint8_t BUTTON_PIN = D2;
const uint8_t LED_PIN = D5;

JLed led = JLed(LED_PIN);
LedManager ledManager(led);

ace_button::AceButton button(BUTTON_PIN, LOW);

const int printerBaudRate = 9600;
// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
SoftwareSerial printerSerial(PRINTER_RX_PIN, PRINTER_TX_PIN);
Tprinter printer(&printerSerial, printerBaudRate);
PrintService printService(printer);

HTTPClient httpClient;
WiFiClientSecure wifiClient;
HttpService httpService(&httpClient, wifiClient);

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void onWiFiConnected(const WiFiEventStationModeGotIP &event) {
    ledManager.SetMode(ConnectedMode);
}

void onWiFiDisconnected(const WiFiEventStationModeDisconnected &event) {
    ledManager.SetMode(ConnectingMode);
}

const String cowAscii = "        __n__n__\n"
                        "  .------`-\\00/-'\n"
                        " /  ##  ## (oo)\n"
                        "/ \\## __   ./\n"
                        "   |//YY \\|/\n"
                        "   |||   |||";

void onButtonClicked() {
    String url = "https://raw.githubusercontent.com/kuhess/test-images/main/001.txt";
    String text = httpService.GetStringFromWithRetry(url);
    if (!text.isEmpty()) {
        printService.PrintAscii(text);
    }
}

void onButtonDoubleClicked() {
    printService.PrintBitmap(rocky, rockyWidth, rockyHeight);
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
    // WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Register event handlers
    stationConnectedHandler = WiFi.onStationModeGotIP(&onWiFiConnected);
    stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onWiFiDisconnected);

    // Led
    ledManager.Setup();

    // Button
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    ace_button::ButtonConfig *buttonConfig = button.getButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterDoubleClick);

    // Printer
    printerSerial.begin(printerBaudRate);
    printer.begin();
    printer.enableDtr(PRINTER_DTR_PIN, HIGH);
    printer.setCodePage(CODEPAGE_CP437);
    printer.setCharset(CHARSET_USA);

    // HTTP
    wifiClient.setInsecure();
    httpClient.useHTTP10(true);
    httpClient.setReuse(false);
}


void loop() {
    ledManager.Update();
    button.check();
}
