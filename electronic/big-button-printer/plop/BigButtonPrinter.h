#ifndef BIG_BUTTON_PRINTER_BIGBUTTONPRINTER_H
#define BIG_BUTTON_PRINTER_BIGBUTTONPRINTER_H

#include "IManager.h"
#include "Config.h"
#include "LedManager.h"

class BigButtonPrinter : public IManager {
public:
    explicit BigButtonPrinter(
            Config config,
            LedManager *ledManager
    );

    Config getConfig();

    void Begin() override;

    void Update() override;

private:
    Config config_;
    LedManager *led_manager_;
};

#endif //BIG_BUTTON_PRINTER_BIGBUTTONPRINTER_H
