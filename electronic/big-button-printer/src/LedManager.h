#ifndef BIG_BUTTON_PRINTER_LEDMANAGER_H
#define BIG_BUTTON_PRINTER_LEDMANAGER_H

#include <jled.h>

enum LedMode {
    ConnectingMode, ConnectedMode
};

class LedManager {
public:
    explicit LedManager(JLed &jled);

    void Setup();

    void Update();

    void SetMode(LedMode mode);

private:
    JLed jled_;
};


#endif //BIG_BUTTON_PRINTER_LEDMANAGER_H
