#ifndef UGUI_OBJECT_H_
#define UGUI_OBJECT_H_

#include "uguicpp.h"
#include "messagehandler.h"

namespace UGUI {

class Object : MessageHandler
{
protected:
    MESSAGE_HANDLER
    Object(UG_U8 type) : _type(type) { }

    virtual UG_OBJECT *getObject(void);

    UG_WINDOW *_wnd;
    UG_U8 _id;
    UG_U8 _type;
};

}

#endif