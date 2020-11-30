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
    };

    typedef std::function<void(Input::msg_t)> input_cb_t;

    void enable(void);
    void loop(void);

    input_cb_t cb;

private:
    int _nextTick;
};

#endif