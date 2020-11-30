#ifndef UI_ROTATING_TEXTBOX_H_
#define UI_ROTATING_TEXTBOX_H_

#include "freertos/FreeRTOS.h"
#include "UGUICPP/textbox.h"

class RotatingTextbox : public UGUI::Textbox
{
public:
    RotatingTextbox() : UGUI::Textbox() { }

    UG_RESULT show(void);
    UG_RESULT setText(std::string str);

    bool loop(void);

    TickType_t initialDelay = 1500 / portTICK_PERIOD_MS;
    TickType_t rotateDelay = 750 / portTICK_PERIOD_MS;

private:
    void rotate(void);
    bool needsRotate(void);

    std::string _origText;
    std::string _rotText;
    TickType_t _nextTick;
};

#endif