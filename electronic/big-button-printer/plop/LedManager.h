#ifndef BIG_BUTTON_PRINTER_LEDMANAGER_H
#define BIG_BUTTON_PRINTER_LEDMANAGER_H

#include <cstdint>

#include "jled.h"

#include "IManager.h"

class LedManager : public IManager {
public:
    explicit LedManager(uint8_t pin);

    void Begin() override;

    void Update() override;

private:
    uint8_t pin_;
    //JLed* led;
};

#endif //BIG_BUTTON_PRINTER_LEDMANAGER_H
