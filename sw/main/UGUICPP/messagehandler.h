#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include "uguicpp.h"

#define MESSAGE_HANDLER friend class MessageHandler;

namespace UGUI {

// This class MUST have zero storage size! No member variables!
class MessageHandler
{
public:
    virtual void handleMessage(UG_MESSAGE *msg) { }

    static void cb(UG_MESSAGE *msg);
};

}

#endif