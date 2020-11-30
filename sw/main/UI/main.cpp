#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "loop.h"

#include "main.h"

UG_RESULT Main::create(void)
{
    Window::create();
    this->setForeColor(15);
    this->setBackColor(0);
    this->setTitleTextColor(15);
    this->setTitleColor(0);
    this->setTitleInactiveTextColor(1);
    this->setTitleInactiveColor(0);
    this->setTitleTextFont(&FONT_12X16);
    this->setTitleTextHSpace(0);
    this->setTitleTextVSpace(0);
    this->setTitleTextAlignment(ALIGN_TOP_CENTER);
    this->setTitleHeight(0);
    this->setStyle(WND_STYLE_2D|WND_STYLE_HIDE_TITLE);

    const UG_FONT *font = &FONT_16X26;

    const int w_s = 5; // widget space
    UG_AREA wa, ta, aa;

    this->getArea(&wa);

    ta.xs = 0;
    ta.ys = w_s;
    ta.xe = wa.xe;
    ta.ye = ta.ys + font->char_height + 2;

    UG_ERROR_CHECK(_title.create(this, TXB_ID_0,
        ta.xs,
        ta.ys,
        ta.xe,
        ta.ye));
    _title.setForeColor(15);
    _title.setBackColor(0);
    _title.setFont(font);
    _title.setHSpace(0);
    _title.setVSpace(0);
    _title.setAlignment(ALIGN_CENTER);
    _title.setText("LONG TITLE");
    _title.show();

#if 0
    UG_ERROR_CHECK(_artist.create(this, TXB_ID_1,
        0, w_s + font->char_height + 2 + w_s,
        127,
        w_s + font->char_height + 2 + w_s + font->char_height + 2));
    _artist.setForeColor(15);
    _artist.setBackColor(0);
    _artist.setFont(font);
    _artist.setHSpace(0);
    _artist.setVSpace(0);
    _artist.setAlignment(ALIGN_CENTER);
    _artist.setText("AAA");
    _artist.show();
#endif

    return UG_RESULT_OK;
}

UG_RESULT Main::show(void)
{
    Loop::registerLoop(std::bind(&Main::loop, this));
    _nextTick = xTaskGetTickCount() + 5000 / portTICK_PERIOD_MS;
    return Window::show();
}

bool Main::loop(void)
{
    if (!(this->_wnd.state & WND_STATE_VISIBLE))
        return false;

    if (_nextTick < xTaskGetTickCount()) {
        _nextTick = xTaskGetTickCount() + 5000 / portTICK_PERIOD_MS;

        // Update from media info?
        _title.setText("Gotcha!");
    }

    return true;
}