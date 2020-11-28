#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
#include "OLED/OLED_Driver.h"
#include "UGUI/ugui.h"
}

#include "display.h"

static UG_GUI g;
static const uint32_t guiUpdateTimeout = 10 / portTICK_PERIOD_MS;

static void Display_setPixel(UG_S16 x,UG_S16 y, UG_COLOR c)
{
    if (c != C_TRANSPARENT)
        OLED_SetColor(x, y, c);
}

Display::Display(void)
{
    const int f_h = 26; // font height
    const int w_s = 5; // widget space

    this->_title.font = &FONT_16X26;
    this->_title.pos = {
        0,
        5,
        127,
        5 + f_h + 2,
    };
    this->_title.padding = { 2, 5, 0, 5 };
    this->_title.textColor = 4;
    this->_title.fillColor = 0;
    this->_title.rotateDelay_ms = 750;
    this->_title.initialDelay_ms = 1500;
    this->_widgets.push_back(&this->_title);

    this->_artist.font = &FONT_16X26;
    this->_artist.pos = {
        0,
        this->_title.pos.y2 + w_s,
        127,
        this->_title.pos.y2 + w_s + f_h + 2,
    };
    this->_artist.padding = this->_title.padding;
    this->_artist.textColor = 4;
    this->_artist.fillColor = 0;
    this->_title.rotateDelay_ms = 750;
    this->_title.initialDelay_ms = 1500;
    this->_widgets.push_back(&this->_artist);
}

void Display::enable(void)
{
    OLED_Sys_Init();
    OLED_Init(SCAN_DIR_DFT);

    UG_Init(&g, &Display_setPixel, OLED_X_MAXPIXEL, OLED_Y_MAXPIXEL);

    this->_nextGuiTick = 0;
}

void Display::loop(void)
{
    if (this->_nextGuiTick <= xTaskGetTickCount()) {
        this->_nextGuiTick = xTaskGetTickCount() + guiUpdateTimeout;
        this->update();
    }
}

void Display::setMediaInfo(MediaInfo &mi)
{
    this->_title.set(mi.getField(MediaInfo::FIELD_TITLE));
    this->_artist.set(mi.getField(MediaInfo::FIELD_ARTIST));
}

void Display::update(void)
{
    OLED_Clear(0x0);
    this->render();
    OLED_Display();
}

void Display::render(void)
{
    for (Widget *w : this->_widgets)
        w->render();
}