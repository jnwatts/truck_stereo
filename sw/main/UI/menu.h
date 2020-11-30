#ifndef UI_MENU_H_
#define UI_MENU_H_

#include "input.h" //TODO: Just need this for Input::msg_t... need a generic input_event_t or something...
#include "window.h"
#include "UGUICPP/textbox.h"

#include <map>

class Menu : public Window
{
public:
    Menu(int max_rows_per_page) : Window(max_rows_per_page) { }

    UG_RESULT create(void);
    UG_RESULT remove(void);

    virtual void handleMessage(UG_MESSAGE *msg);
    virtual void inputEvent(Input::msg_t msg);

    void drawPage(void);
    UG_RESULT layout(void);
    void update(void);

    void next(void);
    void prev(void);

    void setItem(int id, std::string item) { _items[id] = item; };

private:
    std::map<int, std::string> _items;
    std::vector<UGUI::Textbox> _textBoxes;
    int _cursor;
    int _num_rows;
};

#endif