#ifndef TEXTBOX_H_
#define TEXTBOX_H_

#include "uguicpp.h"
#include "window.h"
#include "object.h"

namespace UGUI {

class Textbox : protected Object
{
protected:
    UG_TEXTBOX _txb;
    MESSAGE_HANDLER

public:
    Textbox() : Object(OBJ_TYPE_TEXTBOX) { }
    ~Textbox() { remove(); }

    UG_TEXTBOX *getHandle(void) { return &_txb; }

    virtual UG_RESULT create(Window *wnd, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye) {
        _wnd = wnd->getHandle();
        _id = id;
        return UG_TextboxCreate(_wnd, &_txb, _id, xs, ys, xe, ye);
    }
    virtual UG_RESULT remove() { return UG_TextboxDelete(_wnd, _id); }
    virtual UG_RESULT show() { return UG_TextboxShow(_wnd, _id); }
    virtual UG_RESULT hide() { return UG_TextboxHide(_wnd, _id); }
    virtual UG_RESULT setForeColor(UG_COLOR fc) { return UG_TextboxSetForeColor(_wnd, _id, fc); }
    virtual UG_RESULT setBackColor(UG_COLOR bc) { return UG_TextboxSetBackColor(_wnd, _id, bc); }
    virtual UG_RESULT setText(std::string str) {
        strdup(str, _text);
        return UG_TextboxSetText(_wnd, _id, &_text[0]);
    }
    virtual UG_RESULT setFont(const UG_FONT* font) { return UG_TextboxSetFont(_wnd, _id, font); }
    virtual UG_RESULT setHSpace(UG_S8 hs) { return UG_TextboxSetHSpace(_wnd, _id, hs); }
    virtual UG_RESULT setVSpace(UG_S8 vs) { return UG_TextboxSetVSpace(_wnd, _id, vs); }
    virtual UG_RESULT setAlignment(UG_U8 align) { return UG_TextboxSetAlignment(_wnd, _id, align); }

    virtual UG_COLOR getForeColor(void) { return UG_TextboxGetForeColor(_wnd, _id); }
    virtual UG_COLOR getBackColor(void) { return UG_TextboxGetBackColor(_wnd, _id); }
    virtual std::string getText() { return std::string(&_text[0], _text.size()); }
    virtual const UG_FONT* getFont(void) { return UG_TextboxGetFont(_wnd, _id); }
    virtual UG_S8 getHSpace(void) { return UG_TextboxGetHSpace(_wnd, _id); }
    virtual UG_S8 getVSpace(void) { return UG_TextboxGetVSpace(_wnd, _id); }
    virtual UG_U8 getAlignment(void) { return UG_TextboxGetAlignment(_wnd, _id); }

protected:
    std::vector<char> _text;
};

}

#endif