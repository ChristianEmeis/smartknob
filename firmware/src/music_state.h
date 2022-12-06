#pragma once
#include <string>

#include "lvgl.h"


class MusicState{
    public:
        int volume;
        char led[3*8]; 
        std::string artist;
        std::string title;
        uint8_t *image = (uint8_t *)ps_malloc(115200);
};
