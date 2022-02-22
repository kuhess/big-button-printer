#include "PrintService.h"

PrintService::PrintService(Tprinter &printer) : printer_(printer) {}

void PrintService::PrintAscii(String ascii) {
    printer_.justify('L');
    printer_.println(ascii);
    printer_.feed(3);
}
