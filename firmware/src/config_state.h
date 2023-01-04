#pragma once
#include <string>

#include "lvgl.h"


class ConfigState{
    public:
        int volume;
        char led[3*8]; 
        std::string artist;
        std::string title;
        uint8_t *image = NULL;
        bool imageLoading;
        bool autoBrightness;
        ConfigState();
        ~ConfigState();
};
