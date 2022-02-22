#include <ESP8266WiFi.h>
#include <stdio.h>

#include <jled.h>

#include <LedManager.h>
#include "wifi.h"

#ifndef WIFI_H
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"
#endif

JLed led = JLed(D5);
LedManager ledManager(led);

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

void onWiFiConnected(const WiFiEventStationModeGotIP &event) {
    ledManager.SetMode(ConnectedMode);
}

void onWiFiDisconnected(const WiFiEventStationModeDisconnected &event) {
    ledManager.SetMode(ConnectingMode);
}

void setup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // Register event handlers
    stationConnectedHandler = WiFi.onStationModeGotIP(&onWiFiConnected);
    stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onWiFiDisconnected);

    ledManager.Setup();
}


void loop() {
    ledManager.Update();
}
