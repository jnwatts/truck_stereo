#include <rotary_encoder.h>

#include "input.h"

static const gpio_num_t ROT_A = GPIO_NUM_32;
static const gpio_num_t ROT_B = GPIO_NUM_33;
static const gpio_num_t ROT_SW = GPIO_NUM_26;

static const int rotDelay = 100 / portTICK_PERIOD_MS;
static const int debounceDelay = 10 / portTICK_PERIOD_MS;
static const int longSelectDelay = 500 / portTICK_PERIOD_MS;

enum debounce_t {
    DEBOUNCE_IDLE = 0,
    DEBOUNCE_WAIT_FOR_LONG_OR_DEPRESS,
    DEBOUNCE_WAIT_FOR_DEPRESS,
};

rotary_encoder_info_t sw;

void Input::enable(void)
{
    // init gpio
    ESP_ERROR_CHECK(gpio_reset_pin(ROT_SW));
    // gpio_pad_select_gpio(ROT_SW);
    ESP_ERROR_CHECK(gpio_set_pull_mode(ROT_SW, GPIO_PULLUP_ONLY));
    ESP_ERROR_CHECK(gpio_set_direction(ROT_SW, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_intr_type(ROT_SW, GPIO_INTR_NEGEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(ROT_SW, &Input::intr, this));

    ESP_ERROR_CHECK(rotary_encoder_init(&sw, GPIO_NUM_32, GPIO_NUM_33));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&sw, true));
}

bool Input::loop(void)
{
    if (this->_swState == DEBOUNCE_IDLE) {
        if (this->_rotTick <= xTaskGetTickCount()) {
            this->_rotTick = xTaskGetTickCount() + rotDelay;

            rotary_encoder_state_t sw_state;
            rotary_encoder_get_state(&sw, &sw_state);
            rotary_encoder_reset(&sw);

            if (sw_state.position != 0)
                this->doMsg((sw_state.position > 0 ? Input::RIGHT : Input::LEFT));
        }
    } else if (this->_swState == DEBOUNCE_WAIT_FOR_LONG_OR_DEPRESS) {
        rotary_encoder_reset(&sw);
        if (gpio_get_level(ROT_SW) == 1 && this->_swTick + debounceDelay <= xTaskGetTickCount()) {
            // Depressed
            this->doMsg(Input::SELECT);
            this->_swState = DEBOUNCE_IDLE;
        } else if ((gpio_get_level(ROT_SW) == 0) && (this->_swTick + longSelectDelay <= xTaskGetTickCount())) {
            // Long press
            this->doMsg(Input::LONG_SELECT);
            this->_swState = DEBOUNCE_WAIT_FOR_DEPRESS;
        }
    } else if (this->_swState == DEBOUNCE_WAIT_FOR_DEPRESS) {
        rotary_encoder_reset(&sw);
        if (gpio_get_level(ROT_SW) == 1)
            this->_swState = DEBOUNCE_IDLE;
    }
    return true;
}

void Input::doMsg(Input::msg_t msg)
{
    if (this->cb)
        this->cb(msg);
}

void Input::intr(void *arg)
{
    Input *self = static_cast<Input*>(arg);
    if (self->_swState == DEBOUNCE_IDLE || self->_swState == DEBOUNCE_WAIT_FOR_LONG_OR_DEPRESS) {
        self->_swState = DEBOUNCE_WAIT_FOR_LONG_OR_DEPRESS;
        self->_swTick = xTaskGetTickCount();
    }
}