#include <assert.h>
#include "menu.h"

UG_RESULT Menu::create(void) {
    Window::create();
    _cursor = 0;
    _num_rows = 0;
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
    this->setTitleHeight(16+2);
    this->setStyle(WND_STYLE_2D|WND_STYLE_HIDE_TITLE);
    return this->layout();
}

UG_RESULT Menu::remove(void) {
    for (int txb_id = 0; txb_id < _num_rows; txb_id++)
        _textBoxes[txb_id].hide();
    UGUI::Gui::update();
    for (int txb_id = 0; txb_id < _num_rows; txb_id++)
        _textBoxes[txb_id].remove();
    return Window::remove();
}

void Menu::handleMessage(UG_MESSAGE *msg) {
    if (msg->event == WND_EVENT_POSTRENDER) {
        UG_AREA a;
        UG_TEXT txt;

        this->getArea(&a);

        txt.fc = this->getTitleTextColor();
        txt.bc = this->getTitleColor();
        txt.str = const_cast<char*>(this->getTitleText().c_str());
        txt.font = this->getTitleTextFont();
        txt.a.xs = a.xs + 1;
        txt.a.ys = a.ys + 1;
        txt.a.xe = a.xe - 1;
        txt.a.ye = a.ys + 1 + this->getTitleHeight();
        txt.align = ALIGN_CENTER;
        txt.h_space = this->getTitleTextHSpace();
        txt.v_space = this->getTitleTextVSpace();
        UG_PutText(&txt);

        this->drawPage();

        UG_DrawLine(a.xs, txt.a.ye + 2, a.xe, txt.a.ye + 2, this->getTitleTextColor());
    }
}

void Menu::inputEvent(Input::msg_t msg)
{
    switch (msg) {
        case Input::LEFT:
            this->prev();
            this->update();
            break;
        case Input::RIGHT:
            this->next();
            this->update();
            break;
        case Input::SELECT:
            //TODO: Callback?
            break;
        case Input::LONG_SELECT:
            //TODO: Callback?
            break;
    }
}

void Menu::drawPage(void)
{
    UG_AREA wa, pa;
    int page = _cursor / _num_rows + 1;
    int pagecount = ((_items.size() - 1) / _num_rows) + 1;
    char buf[4];

    this->getArea(&wa);

    pa.xs = wa.xe - 2 - 4 - 3 - 4;
    pa.xe = wa.xe - 2;
    pa.ys = wa.ys + 2;
    pa.ye = wa.ys + 2 + 6 + 3;

    UGUI::Gui::fontSelect(&FONT_4X6);

    UGUI::Gui::fillFrame(pa.xs, pa.ye, pa.xe, pa.ys, 0);
    UGUI::Gui::setForecolor(this->getTitleTextColor());

    snprintf(buf, sizeof(buf), "%d", page % 10);
    UGUI::Gui::putString(pa.xs, pa.ys, buf);
    snprintf(buf, sizeof(buf), "%d", pagecount % 10);
    UGUI::Gui::putString(pa.xs + 3 + 4, pa.ys + 3, buf);

    UGUI::Gui::drawLine(pa.xs + 4, pa.ye - 1, pa.xe - 4, pa.ys + 1, 1);
}

UG_RESULT Menu::layout(void) {
    const UG_FONT *itemFont = &FONT_12X16;

    for (int txb_id = 0; txb_id < _num_rows; txb_id++)
        _textBoxes[txb_id].hide();
    UGUI::Gui::update();
    for (int txb_id = 0; txb_id < _num_rows; txb_id++)
        _textBoxes[txb_id].remove();

    UG_AREA a;
    this->getArea(&a);
    const int title_height = this->getTitleHeight() + 2 /* around text */ + 1 /* bar */;
    const int inner_width = (a.xe - a.xs);
    const int inner_height = (a.ye - a.ys) - title_height;
    const int item_h_pad = 0;
    const int item_v_pad = 5;
    const int font_height = itemFont->char_height;
    const int item_width = inner_width - 2 * item_h_pad;
    const int item_height = font_height + 2 * item_v_pad;
    const int item_v_spacing = 2;
    _num_rows = (inner_height - item_v_pad * 2) / (item_height + item_v_spacing);
    assert(_num_rows <= _objlst.size());

    _textBoxes.resize(_num_rows);
    for (int txb_id = 0; txb_id < _num_rows; txb_id++) {
        UGUI::Textbox &txt = _textBoxes[txb_id];
        UG_ERROR_CHECK(txt.create(this, txb_id,
            item_h_pad,
            title_height + item_v_pad + (item_height + item_v_spacing) * txb_id,
            item_h_pad + item_width,
            title_height + item_v_pad + (item_height + item_v_spacing) * txb_id + item_height));
        txt.setForeColor(15);
        txt.setBackColor(0);
        txt.setFont(itemFont);
        txt.setHSpace(0);
        txt.setVSpace(0);
        txt.setAlignment(ALIGN_CENTER_LEFT);
        txt.show();
    }
    this->update();
    return UG_RESULT_OK;
}

void Menu::update(void) {
    int selected = _cursor % _num_rows;
    int page = _cursor / _num_rows;
    if (_items.size() == 0)
        return;
    for (int txb_id = 0; txb_id < _num_rows; txb_id++) {
        if (txb_id == selected)
            _textBoxes[txb_id].setBackColor(1);
        else
            _textBoxes[txb_id].setBackColor(0);
        auto iter = _items.find(page * _num_rows + txb_id);
        if (iter != _items.end())
            _textBoxes[txb_id].setText(iter->second);
        else
            _textBoxes[txb_id].setText("");
    }
    this->drawPage();
}

void Menu::next(void) {
    auto iter = _items.find(_cursor);
    do {
        if (iter != _items.end()) {
            iter++;
            if (iter == _items.end())
                return;

            if (!iter->second.empty()) {
                _cursor = iter->first;
                return;
            }

            // Not valid, try next
        }
    } while (iter != _items.end());
    // No valid entries found, leave _cursor alone
}

void Menu::prev(void) {
    auto iter = _items.find(_cursor);
    do {
        if (iter != _items.begin()) {
            iter--;
            if (!iter->second.empty()) {
                _cursor = iter->first;
                return;
            }
            // Not valid, try next
        }
    } while (iter != _items.begin());
}