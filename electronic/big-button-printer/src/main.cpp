#include <Arduino.h>

#include <AceButton.h>
using namespace ace_button;

const int BUTTON_PIN = D2;
const int LED_PIN = D5;

AceButton button(BUTTON_PIN, LOW);

// Forward reference to prevent Arduino compiler becoming confused.
void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  button.setEventHandler(handleEvent);
}

void loop() {
  button.check();
}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("PRESSED");
      digitalWrite(LED_PIN, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("RELEASED");
      digitalWrite(LED_PIN, LOW);
      break;
  }
}
