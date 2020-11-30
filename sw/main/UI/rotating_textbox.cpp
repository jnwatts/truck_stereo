#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "loop.h"

#include "rotating_textbox.h"

UG_RESULT RotatingTextbox::show(void)
{
    Loop::registerLoop<RotatingTextbox>(this);
    _nextTick = xTaskGetTickCount() + initialDelay;
    _rotText = std::string();
    this->rotate();
    return UGUI::Textbox::show();
}

UG_RESULT RotatingTextbox::setText(std::string str)
{
    _origText = str;
    _rotText = std::string();
    return UGUI::Textbox::setText(str);
}

bool RotatingTextbox::loop(void)
{
    UG_OBJECT *obj = this->getObject();
    if (!obj || !(obj->state & OBJ_STATE_VISIBLE))
        return false;

    if (_nextTick < xTaskGetTickCount()) {
        _nextTick = xTaskGetTickCount() + rotateDelay;

        this->rotate();
    }

    return true;
}

void RotatingTextbox::rotate(void)
{
    // find max length for current size
    UG_OBJECT *obj = this->getObject();
    if (!obj)
        return;

    const UG_FONT *font = this->getFont();
    int str_len = 0;
    int str_w = 0;
    int max_w = obj->a_rel.xe - obj->a_rel.xs;
    while (str_len < _origText.size()) {
        int char_width = UGUI::actual_char_width(font, _origText[str_len]);
        if (str_w + char_width > max_w)
            break;
        str_w += char_width;
        str_len++;
    }

    // Can we show the full string as is currently set?
    if (str_len >= _origText.size()) {
        // Yup, show the full string
        UGUI::Textbox::setText(_origText);
    } else {
        // Nope, check if _rotText needs to be initialized
        if (_rotText.empty())
            _rotText = _origText + "\xB3";

        // Show visible sub-string of _rotText
        UGUI::Textbox::setText(_rotText.substr(0, str_len));

        // Rotate _rotText
        char tmp = _rotText[0];
        _rotText.erase(0, 1);
        _rotText.push_back(tmp);
    }
}

bool RotatingTextbox::needsRotate(void)
{
    UG_OBJECT *obj = this->getObject();
    if (obj && UGUI::actual_str_width(this->getFont(), _origText) > (obj->a_rel.xe - obj->a_rel.xs))
        return true;
    return false;
}
