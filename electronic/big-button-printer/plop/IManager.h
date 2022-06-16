#ifndef BIG_BUTTON_PRINTER_IMANAGER_H
#define BIG_BUTTON_PRINTER_IMANAGER_H

class IManager {
protected:
    IManager() = default;
    ~IManager() = default;

public:
    virtual void Begin() = 0;
    virtual void Update() = 0;
};

#endif //BIG_BUTTON_PRINTER_IMANAGER_H
