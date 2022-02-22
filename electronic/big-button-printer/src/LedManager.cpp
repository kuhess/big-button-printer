#include "LedManager.h"

LedManager::LedManager(JLed &jled) : jled_(jled) {}

void LedManager::Setup() {
    SetMode(ConnectingMode);
}


void LedManager::Update() {
    jled_.Update();
}

void LedManager::SetMode(LedMode mode) {
    switch (mode) {
        case ConnectingMode:
            jled_.Blink(250, 250).Forever();
            break;
        case ConnectedMode:
            jled_.Breathe(4000).Forever();
            break;
        default:
            jled_.Off();
    }
}
