#include <rotary_encoder.h>

#include "input.h"

rotary_encoder_info_t sw;

static const int inputDelay = 100 / portTICK_PERIOD_MS;

void Input::enable(void)
{
    rotary_encoder_init(&sw, GPIO_NUM_32, GPIO_NUM_33);
    rotary_encoder_enable_half_steps(&sw, true);
}

void Input::loop(void)
{
    if (this->_nextTick > xTaskGetTickCount())
        return;

    this->_nextTick = xTaskGetTickCount() + inputDelay;

    rotary_encoder_state_t sw_state;
    rotary_encoder_get_state(&sw, &sw_state);
    rotary_encoder_reset(&sw);

    if (this->cb && sw_state.position != 0)
        this->cb((sw_state.position > 0 ? Input::RIGHT : Input::LEFT));
}