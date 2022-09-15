#include <Arduino.h>
#include <SoftwareSerial.h>

#include <AceButton.h>
#include <jled.h>
#include <LittleFS.h>

#include "TPrinter.h"


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


void setupPrinter() {
    mySerial.begin(printerBaudrate);
    printer.begin();
    printer.enableDtr(dtrPin, HIGH);
    printer.setCodePage(CODEPAGE_CP437);
    printer.setCharset(CHARSET_USA);
}

void onButtonClicked() {
    printer.println("Click");
    // File image_file = LittleFS.open("/384.raw", "r");
    // printer.printRawImage(&image_file, 384);
}

void onButtonDoubleClicked() {
    printer.println("Double click");
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
    LittleFS.begin();

    setupPrinter();

    // Button & LED
    pinMode(led_pin, OUTPUT);
    pinMode(button_pin, INPUT_PULLUP);

    ace_button::ButtonConfig *buttonConfig = button.getButtonConfig();
    buttonConfig->setEventHandler(handleEvent);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterClick);
    buttonConfig->setFeature(ace_button::ButtonConfig::kFeatureSuppressAfterDoubleClick);

    led.Breathe(2000).Forever();
}

void loop() {
    button.check();
    led.Update();
}
