#ifndef INPUT_H_
#define INPUT_H_


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

    typedef void (*input_cb_t)(Input::msg_t msg);

    void enable(void);
    void loop(void);

    input_cb_t cb;

private:
    int _nextTick;
};

#endif