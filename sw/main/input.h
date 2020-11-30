#ifndef INPUT_H_
#define INPUT_H_

#include <functional>

class Input
{
public:
    Input() = default;
    ~Input() = default;

    enum msg_t {
        LEFT = 0,
        RIGHT,
        SELECT,
        LONG_SELECT,
    };

    typedef std::function<void(Input::msg_t)> input_cb_t;

    void enable(void);
    bool loop(void);

    input_cb_t cb;

private:
    void doMsg(Input::msg_t msg);
    static void intr(void *arg);

    int _rotTick = 0;
    int _swTick = 0;
    int _swState = 0;
};

#endif