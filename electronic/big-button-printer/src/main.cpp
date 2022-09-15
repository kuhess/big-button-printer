#include <Arduino.h>
#include <SoftwareSerial.h>

#include <LittleFS.h>

#include "TPrinter.h"


const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;
// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
// /!\ DO NOT USE SERIAL TO LOG, ONLY FOR PRINTER
SoftwareSerial mySerial(rxPin, txPin);
Tprinter printer(&mySerial, printerBaudrate);

void setupPrinter() {
    mySerial.begin(printerBaudrate);
    printer.begin();
    printer.enableDtr(dtrPin, HIGH);
    printer.setCodePage(CODEPAGE_CP437);
    printer.setCharset(CHARSET_USA);
}

void setup() {
    setupPrinter();

    LittleFS.begin();

    File image_file = LittleFS.open("/quentin.raw", "r");
    printer.printRawImage(&image_file, 384);
}

void loop() {
    delay(100);
}
