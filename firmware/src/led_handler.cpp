#pragma once
#include <Arduino.h>
#include "led_handler.h"

#if SK_LEDS
#include <FastLED.h>
#endif

#if SK_LEDS
CRGB led_data_store[NUM_LEDS];
#endif

void LedHandler::run(){
    #if SK_LEDS
        FastLED.addLeds<SK6812, PIN_LED_DATA, GRB>(led_data_store, NUM_LEDS);
    #endif
}

void LedHandler::changeLed(uint8_t *led_data){
    #if SK_LEDS
        int offset = 0;
        for (uint8_t i = 0; i < NUM_LEDS; i++) {
                //WEIRD IF STATEMENT DUE TO ROTATION (FIRST ADDRESSABLE LED IS NOT AT THE TOP RIGHT)
            if(i==6){
                led_data_store[0].setRGB(led_data[3*offset], led_data[3*offset+1], led_data[3*offset+2]);
            } else if(i==7){
                led_data_store[1].setRGB(led_data[3*offset], led_data[3*offset+1], led_data[3*offset+2]);
            }
            led_data_store[i+2].setRGB(led_data[3*offset], led_data[3*offset+1], led_data[3*offset+2]);
            offset++;
        }
        FastLED.setBrightness(100);
        FastLED.setCorrection(CRGB( 255, 90, 180));
        FastLED.show();
        FastLED.setBrightness(100);
    #endif
}

void LedHandler::changeBrightness(uint8_t brightness){
    #if SK_LEDS
        FastLED.setBrightness(brightness);
    #endif
}