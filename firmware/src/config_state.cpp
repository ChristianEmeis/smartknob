#include "config_state.h"

#include <Arduino.h>

ConfigState::ConfigState(){
    this->image = (uint8_t *) ps_malloc(115200 * sizeof(uint8_t));
}

ConfigState::~ConfigState(){
    free(this->image);
}