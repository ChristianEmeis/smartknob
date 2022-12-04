#pragma once
#include <Arduino.h>
class LedHandler {
    public:
        void run();
        void changeLed(uint8_t *led_data);
        void changeBrightness(uint8_t brightness);
};