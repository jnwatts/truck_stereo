#include "messagehandler.h"
#include "window.h"
// #include "button.h"
#include "textbox.h"
// #include "image.h"
// #include "checkbox.h"

using namespace UGUI;

void MessageHandler::cb(UG_MESSAGE *msg) {
    MessageHandler *self = nullptr;
    UG_OBJECT *obj;

    switch (msg->type) {
        case MSG_TYPE_WINDOW:
            self = container_of((UG_WINDOW*)msg->src, Window, _wnd);
            break;
        case MSG_TYPE_OBJECT:
            obj = reinterpret_cast<UG_OBJECT*>(msg->src);
            switch (obj->type) {
                // case OBJ_TYPE_BUTTON: self = container_of(obj->data, ); break;
                case OBJ_TYPE_TEXTBOX: self = container_of((UG_TEXTBOX*)obj->data, Textbox, _txb); break;
                // case OBJ_TYPE_IMAGE: self = container_of(obj->data, ); break;
                // case OBJ_TYPE_CHECKBOX: self = container_of(obj->data, ); break;
            }
            break;
    }


    if (self)
        self->handleMessage(msg);
}
