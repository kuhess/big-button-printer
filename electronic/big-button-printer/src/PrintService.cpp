#include "PrintService.h"

PrintService::PrintService(Tprinter &printer) : printer_(printer) {}

void PrintService::PrintAscii(String ascii) {
    printer_.justify('L');
    printer_.println(ascii);
    printer_.feed(3);
}

void PrintService::PrintBitmap(uint8_t *bitmap, uint16_t width, uint16_t height) {
    printer_.printBitmap(bitmap, width, height, 1);
    printer_.feed(3);
}