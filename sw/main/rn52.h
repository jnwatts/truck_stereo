/*
Copyright (c) 2020 Josh Watts

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef RN52_H_
#define RN52_H_

#include <driver/uart.h>
#include "mediainfo.h"

class RN52
{
public:
    RN52() = default;
    ~RN52() = default;

    enum event_t {
        EVENT_CONNECTION,
        EVENT_STREAMING,
    };

    typedef void (*event_cb_t)(event_t event, void *arg);

    void enable(void);
    void disable(void);
    bool loop(void);
    bool getMediaInfo(MediaInfo &info);
    void connect(void);
    void disconnect(void);
    void eraseConnections(void);
    void trackNext(void);
    void trackPrev(void);

    void setStatusCallback(event_cb_t cb, void *arg);
    bool connected(void);
    bool streaming(void);

private:
    void initGpio(void);
    void setPower(bool enabled);
    void setEnabled(bool enabled);
    bool setCommand(bool enabled);
    bool setParam(char param, const char *value);
    bool getParam(char param);
    const char *paramValue(void);
    bool getStatus(uint16_t *status);
    bool doCmd(const char *cmd, const char *expected = "");
    bool dumpCmd(const char *cmd);
    int readLine(void);
    bool expect(const char *expected);
    static void intr(void *arg);
    void decodeStatus(uint16_t status);
    unsigned connState(void);

    bool _enabled;
    bool _commandMode;
    char _line[32];
    int _lastStatus;
    QueueHandle_t _uartQueue;
    uint32_t _nextTick;
    event_cb_t _statusCallback;
    void *_statusCallbackArg;
    int _isrCount;
};

#endif