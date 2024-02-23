#pragma once

#include <Arduino.h>

#ifndef BLED
#define BLED LED_BUILTIN
#endif

#define ToggleLED digitalWrite(BLED, !digitalRead(BLED));
#define LEDoff digitalWrite(BLED, HIGH);
#define LEDon digitalWrite(BLED, LOW);

void blink(uint8_t t);
