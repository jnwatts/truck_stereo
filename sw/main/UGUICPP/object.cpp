#include "window.h"

#include "object.h"

using namespace UGUI;

UG_OBJECT *Object::getObject(void)
{
    if (_wnd) return Window::fromHandle(_wnd)->searchObject(_type, _id);
    return nullptr;
}