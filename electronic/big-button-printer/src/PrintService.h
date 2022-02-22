#ifndef BIG_BUTTON_PRINTER_PRINTSERVICE_H
#define BIG_BUTTON_PRINTER_PRINTSERVICE_H

#include <TPrinter.h>

class PrintService {
public:
    explicit PrintService(Tprinter &printer);

    void PrintAscii(String ascii);

private:
    Tprinter printer_;
};


#endif //BIG_BUTTON_PRINTER_PRINTSERVICE_H
