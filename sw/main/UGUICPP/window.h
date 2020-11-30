#ifndef WINDOW_H_
#define WINDOW_H_

#include "uguicpp.h"
#include "messagehandler.h"

//TODO: UGUI namespace?

namespace UGUI {

class Window : MessageHandler
{
protected:
    UG_WINDOW _wnd; // THIS MUST ALWAYS BE THE FIRST ELEMENT!
    MESSAGE_HANDLER

public:
    Window(int objcnt = 1) : _objlst(objcnt) { }
    ~Window() { remove(); }

    UG_WINDOW *getHandle(void) { return &_wnd; }

    static Window *fromHandle(UG_WINDOW *wnd) { return container_of(wnd, Window, _wnd); }

    static Window *getActive(void) {
        UG_WINDOW *wnd;
        if (UG_WindowGetActive(&wnd) == UG_RESULT_OK)
            return fromHandle(wnd);
        else
            return nullptr;
    }

    virtual UG_RESULT create(void) { return UG_WindowCreate(&_wnd, &_objlst[0], _objlst.size(), MessageHandler::cb); }
    virtual UG_RESULT remove(void) { return UG_WindowDelete(&_wnd); }

    virtual UG_RESULT show(void) { return UG_WindowShow(&_wnd); }
    virtual UG_RESULT hide(void) { return UG_WindowHide(&_wnd); }
    virtual UG_RESULT resize(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye) { return UG_WindowResize(&_wnd, xs, ys, xe, ye); }
    virtual UG_RESULT alert(void) { return UG_WindowAlert(&_wnd); }
    virtual UG_RESULT setForeColor(UG_COLOR fc) { return UG_WindowSetForeColor(&_wnd, fc); }
    virtual UG_RESULT setBackColor(UG_COLOR bc) { return UG_WindowSetBackColor(&_wnd, bc); }
    virtual UG_RESULT setTitleTextColor(UG_COLOR c) { return UG_WindowSetTitleTextColor(&_wnd, c); }
    virtual UG_RESULT setTitleColor(UG_COLOR c) { return UG_WindowSetTitleColor(&_wnd, c); }
    virtual UG_RESULT setTitleInactiveTextColor(UG_COLOR c) { return UG_WindowSetTitleInactiveTextColor(&_wnd, c); }
    virtual UG_RESULT setTitleInactiveColor(UG_COLOR c) { return UG_WindowSetTitleInactiveColor(&_wnd, c); }
    virtual UG_RESULT setTitleText(std::string str) {
        strdup(str, _title);
        return UG_WindowSetTitleText(&_wnd, &_title[0]);
    }
    virtual UG_RESULT setTitleTextFont(const UG_FONT* font) { return UG_WindowSetTitleTextFont(&_wnd, font); }
    virtual UG_RESULT setTitleTextHSpace(UG_S8 hs) { return UG_WindowSetTitleTextHSpace(&_wnd, hs); }
    virtual UG_RESULT setTitleTextVSpace(UG_S8 vs) { return UG_WindowSetTitleTextVSpace(&_wnd, vs); }
    virtual UG_RESULT setTitleTextAlignment(UG_U8 align) { return UG_WindowSetTitleTextAlignment(&_wnd, align); }
    virtual UG_RESULT setTitleHeight(UG_U8 height) { return UG_WindowSetTitleHeight(&_wnd, height); }
    virtual UG_RESULT setXStart(UG_S16 xs) { return UG_WindowSetXStart(&_wnd, xs); }
    virtual UG_RESULT setYStart(UG_S16 ys) { return UG_WindowSetYStart(&_wnd, ys); }
    virtual UG_RESULT setXEnd(UG_S16 xe) { return UG_WindowSetXEnd(&_wnd, xe); }
    virtual UG_RESULT setYEnd(UG_S16 ye) { return UG_WindowSetYEnd(&_wnd, ye); }
    virtual UG_RESULT setStyle(UG_U8 style) { return UG_WindowSetStyle(&_wnd, style); }

    virtual UG_COLOR getForeColor() { return UG_WindowGetForeColor(&_wnd); }
    virtual UG_COLOR getBackColor() { return UG_WindowGetBackColor(&_wnd); }
    virtual UG_COLOR getTitleTextColor() { return UG_WindowGetTitleTextColor(&_wnd); }
    virtual UG_COLOR getTitleColor() { return UG_WindowGetTitleColor(&_wnd); }
    virtual UG_COLOR getTitleInactiveTextColor() { return UG_WindowGetTitleInactiveTextColor(&_wnd); }
    virtual UG_COLOR getTitleInactiveColor() { return UG_WindowGetTitleInactiveColor(&_wnd); }
    virtual std::string getTitleText() { return std::string(&_title[0], _title.size()); }
    virtual const UG_FONT* getTitleTextFont() { return UG_WindowGetTitleTextFont(&_wnd); }
    virtual UG_S8 getTitleTextHSpace() { return UG_WindowGetTitleTextHSpace(&_wnd); }
    virtual UG_S8 getTitleTextVSpace() { return UG_WindowGetTitleTextVSpace(&_wnd); }
    virtual UG_U8 getTitleTextAlignment() { return UG_WindowGetTitleTextAlignment(&_wnd); }
    virtual UG_U8 getTitleHeight() { return UG_WindowGetTitleHeight(&_wnd); }
    virtual UG_S16 getXStart() { return UG_WindowGetXStart(&_wnd); }
    virtual UG_S16 getYStart() { return UG_WindowGetYStart(&_wnd); }
    virtual UG_S16 getXEnd() { return UG_WindowGetXEnd(&_wnd); }
    virtual UG_S16 getYEnd() { return UG_WindowGetYEnd(&_wnd); }
    virtual UG_U8 getStyle() { return UG_WindowGetStyle(&_wnd); }
    virtual UG_RESULT getArea(UG_AREA* a) { return UG_WindowGetArea(&_wnd, a); }
    virtual UG_S16 getInnerWidth() { return UG_WindowGetInnerWidth(&_wnd); }
    virtual UG_S16 getOuterWidth() { return UG_WindowGetOuterWidth(&_wnd); }
    virtual UG_S16 getInnerHeight() { return UG_WindowGetInnerHeight(&_wnd); }
    virtual UG_S16 getOuterHeight() { return UG_WindowGetOuterHeight(&_wnd); }

    virtual UG_OBJECT *searchObject(UG_U8 type, UG_U8 id) {
        for (UG_OBJECT &obj : _objlst) {
           if ( !(obj.state & OBJ_STATE_FREE) && (obj.state & OBJ_STATE_VALID) ) {
              if ( (obj.type == type) && (obj.id == id) ) {
                 /* Requested object found! */
                 return &obj;
              }
           }
        }
        return nullptr;
    }

protected:
    std::vector<UG_OBJECT> _objlst;
    std::vector<char> _title;
};

} // UGUI

#endif