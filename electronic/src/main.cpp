#include <Arduino.h>
#include <SoftwareSerial.h>

#include "TPrinter.h"
#include "rocky.h"


const byte dtrPin = D1;  // GPIO 5
const byte rxPin = D9;  // GPIO 3
const byte txPin = D10;  // GPIO 1
const int printerBaudrate = 9600;

// RX, TX are the same as hardware Serial
// I declare a custom SoftwareSerial to not get ESP8266 Watchdog Timer logs
SoftwareSerial mySerial(rxPin, txPin);
Tprinter myPrinter(&mySerial, printerBaudrate);

void setup() {
  // Printer
  mySerial.begin(printerBaudrate);
  myPrinter.begin();
  myPrinter.enableDtr(dtrPin, HIGH);

  myPrinter.justify('C');

  myPrinter.printBitmap(rocky, rockyWidth, rockyHeight, 1);
  myPrinter.feed(1);
  myPrinter.println("Rocky");
  myPrinter.feed(3);
  myPrinter.offline();
}

void loop() {
}
