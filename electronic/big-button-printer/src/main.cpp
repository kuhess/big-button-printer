#include <Arduino.h>
#include <SoftwareSerial.h>

#include <TPrinter.h>

#include "img.h"


const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;
// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
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
  Serial.begin(115200);

  setupPrinter();

  printer.printBitmap(rocky, imgWidth, imgHeight, 1);
  printer.feed(1);
}

void loop() {
  delay(30000);
}
