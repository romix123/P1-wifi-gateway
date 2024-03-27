#include "led.h"

void blink(uint8_t t) {
  for (uint8_t i = 0; i <= t; i++) {
    LEDon;      // Signaal naar laag op ESP-M3
    delay(200); // wacht 200 millisec
    LEDoff;     // LEDoff, signaal naar hoog op de ESP-M3
  }
}