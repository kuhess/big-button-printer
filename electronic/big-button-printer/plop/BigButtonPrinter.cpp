#include "BigButtonPrinter.h"


BigButtonPrinter::BigButtonPrinter(Config config, LedManager* led) : config_(config), led_manager_(led) {
}

Config BigButtonPrinter::getConfig() {
    return config_;
}

void BigButtonPrinter::Begin() {
    led_manager_->Begin();
}

void BigButtonPrinter::Update() {
    led_manager_->Update();
}
