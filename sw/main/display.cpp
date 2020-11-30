#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
#include "OLED/OLED_Driver.h"
}

#include "UGUICPP/uguicpp.h"
#include "UGUICPP/window.h"

#define MENU_TEST
#ifdef MENU_TEST
#include "UI/menu.h"
Menu menu(8);
void menu_test() {
    menu.create();
    menu.fullScreen();
    menu.setTitleText("Title");
    menu.setItem(0, "One");
    menu.setItem(1, "Two");
    menu.setItem(2, "Three");
    menu.setItem(3, "Four");
    menu.setItem(4, "Five");
    menu.setItem(5, "Six");
    menu.layout();
    menu.show();
}
#else
#define menu_test() (void)0
#endif

#define MAIN_TEST
#ifdef MAIN_TEST
#include "UI/main.h"
Main main_window;
void main_test() {
    main_window.create();
    main_window.fullScreen();
    main_window.show();
}
#else
#define main_test() (void)0
#endif


#include "display.h"

static const uint32_t guiUpdateTimeout = 10 / portTICK_PERIOD_MS;

Display *Display::_instance = nullptr;

Display *Display::getInstance(void)
{
    if (!_instance)
        _instance = new Display();
    return _instance;
}

Display::Display(void) :
    Gui(Display::setPixel, OLED_X_MAXPIXEL, OLED_Y_MAXPIXEL)
{
    this->g.desktop_color = 0;
    this->resetDirty();
}

void Display::enable(void)
{
    OLED_Sys_Init();
    OLED_Init(SCAN_DIR_DFT);

    OLED_Clear(0);
    OLED_Display();

    // TODO: Create UI elements
    menu_test();
    main_test();

    this->_nextGuiTick = 0;
}

bool Display::loop(void)
{
    if (this->_nextGuiTick <= xTaskGetTickCount()) {
        Gui::update();

        if (this->_dirty.xs < OLED_X_MAXPIXEL) {
            OLED_DisWindow(
                this->_dirty.xs,
                this->_dirty.ys,
                this->_dirty.xe,
                this->_dirty.ye);
            this->resetDirty();
        }

        this->_nextGuiTick = xTaskGetTickCount() + guiUpdateTimeout;
    }
    return true;
}

void Display::setMediaInfo(MediaInfo &mi)
{
    // this->_title.set(mi.getField(MediaInfo::FIELD_TITLE));
    // this->_artist.set(mi.getField(MediaInfo::FIELD_ARTIST));
}

void Display::inputEvent(Input::msg_t msg)
{
    UGUI::Window *w = UGUI::Window::getActive();
    if (w)
        static_cast<Window *>(w)->inputEvent(msg);
    if (msg == Input::SELECT) {
        if (w)
            w->hide();
        menu.show();
    } else if (msg == Input::LONG_SELECT) {
        if (w)
            w->hide();
        main_window.show();
    }
}

#define C_TRANSPARENT 0xFF

void Display::setPixel(UG_S16 x,UG_S16 y, UG_COLOR c)
{
    Display *self = Display::getInstance();
    //TODO: Instead of single dirty bit, expand area of redraw from {0,0}x{0,0}
    if (c != C_TRANSPARENT) {
        if (x < 0 || y < 0 || x >= OLED_X_MAXPIXEL || y >= OLED_Y_MAXPIXEL)
            return;
        OLED_SetColor(x, y, c);

        if (x < self->_dirty.xs)
            self->_dirty.xs = x;

        if (x > self->_dirty.xe)
            self->_dirty.xe = x;

        if (y < self->_dirty.ys)
            self->_dirty.ys = y;

        if (y > self->_dirty.ye)
            self->_dirty.ye = y;
    }
}

void Display::resetDirty(void)
{
    _dirty = {OLED_X_MAXPIXEL, OLED_Y_MAXPIXEL, 0, 0};
}