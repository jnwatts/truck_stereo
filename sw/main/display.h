#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <list>
#include "input.h"
#include "mediainfo.h"
#include "UGUICPP/uguicpp.h"

#define RIGHT_TRIANGLE "\x10"
#define LEFT_TRIANGLE "\x11"
#define RIGHT_ARROW "\x1A"
#define LEFT_ARROW "\x1B"
#define RIGHT_ANGLE_QUOTE "\xAF"
#define LEFT_ANGLE_QUOTE "\xAE"
#define VERT_PIPE "\xB3"


class Display : public UGUI::Gui {
protected:
    Display();

public:
    static Display *getInstance();

    void enable(void);
    bool loop(void);

    void setMediaInfo(MediaInfo &mi);
    void inputEvent(Input::msg_t msg);

private:
    static void setPixel(UG_S16 x,UG_S16 y, UG_COLOR c);
    void resetDirty(void);

    static Display *_instance;
    uint32_t _nextGuiTick;
    // TextBox _artist; // TODO: Move to MainWindow
    // TextBox _title; // TODO: Move to MainWindow
    UG_AREA _dirty;
};

#endif
