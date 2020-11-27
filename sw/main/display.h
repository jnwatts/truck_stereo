#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <list>
#include "mediainfo.h"
#include "textbox.h"

class Display {
public:
    Display();
    ~Display() = default;

    void enable(void);
    void loop(void);

    void setMediaInfo(MediaInfo &mi);

private:
    void update(void);
    void render(void);

    uint32_t _nextGuiTick;
    std::list<Widget*> _widgets;
    TextBox _artist;
    TextBox _title;
};

#endif
