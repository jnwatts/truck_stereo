#ifndef LOOP_H_
#define LOOP_H_

#include <list>
#include <functional>

class Loop
{
protected:
    Loop() { }

public:
    typedef std::function<bool(void)> cb_t;

    static Loop *getInstance(void) { if (!_instance) _instance = new Loop(); return _instance; }

    static void loop(void) {
        auto cb = _callbacks.begin();
        while (cb != _callbacks.end()) {
            if (!(*cb)())
                _callbacks.erase(cb++);
            else
                ++cb;
        }
    }

    template <class C>
    static void registerLoop(C *obj) {
        registerLoop(std::bind(&C::loop, obj));
    }

    static void registerLoop(cb_t cb) {
        _callbacks.push_back(cb);
    }

protected:
    static Loop *_instance;
    static std::list<cb_t> _callbacks;
};

#endif