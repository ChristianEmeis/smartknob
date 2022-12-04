#pragma once
#include <string>

#include "lvgl.h"


class MusicState{
    public:
        int volume;
        char led[3*8]; 
        std::string artist;
        std::string title;
        uint8_t image[115200];
        //const lv_img_dsc_t image_c_array = {
        //    {LV_IMG_CF_TRUE_COLOR,
        //    0,
        //    0,
        //    240,
        //    240},
        //    7200 * LV_COLOR_SIZE,
        //    image
        //};
};
