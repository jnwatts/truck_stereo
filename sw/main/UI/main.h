#ifndef UI_MAIN_H_
#define UI_MAIN_H_ value

#include "freertos/FreeRTOS.h"
#include "window.h"
#include "rotating_textbox.h"

class Main : public Window
{
public:
    Main() : Window(4) { }

    UG_RESULT create(void);
    UG_RESULT show(void);

    bool loop(void);


private:
    RotatingTextbox _title;
    RotatingTextbox _artist;
    TickType_t _nextTick;
};

#endif