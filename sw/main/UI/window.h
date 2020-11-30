#ifndef UI_WINDOW_H_
#define UI_WINDOW_H_

#include "input.h"
#include "UGUICPP/window.h"

class Window : public UGUI::Window
{
public:
    Window(int objcnt = 1) : UGUI::Window(objcnt) { };
    ~Window() = default;

    virtual void inputEvent(Input::msg_t msg) { }

    void fullScreen(void) { this->resize(0, 0, UGUI::Gui::getXDim() - 1, UGUI::Gui::getXDim() - 1); }
};

#endif