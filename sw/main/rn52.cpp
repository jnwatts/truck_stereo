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

#include <ctype.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rn52.h"

// #define DEBUG_STATUS
// #define DEBUG_INFINITE_ENABLE_TIMEOUT

static const gpio_num_t rn52_tx_pin = GPIO_NUM_21;
static const gpio_num_t rn52_rx_pin = GPIO_NUM_22;
static const gpio_num_t rn52_vdd_sw_pin = GPIO_NUM_27;
static const gpio_num_t rn52_cmdn_pin = GPIO_NUM_25;
static const gpio_num_t rn52_pwren_pin = GPIO_NUM_14;
static const gpio_num_t rn52_frst_pin = GPIO_NUM_17;
static const gpio_num_t rn52_int_pin = GPIO_NUM_16;

static const int rn52_uart_num = UART_NUM_2;
static const int rn52_uart_buff_size = 512;

static const uint32_t powerUpTimeout = 3000 / portTICK_PERIOD_MS;
static const uint32_t charTimeout = 250 / portTICK_PERIOD_MS;
static const uint32_t rn52_loop_delay = 500 / portTICK_PERIOD_MS;

static const char *EMPTY_STR = "";

enum profile_t {
    PROFILE_IAP = (1u << 0),
    PROFILE_SPP = (1u << 1),
    PROFILE_A2DP = (1u << 2),
    PROFILE_HFP_HSP = (1u << 3),
    PROFILE_CID = (1u << 4),
    PROFILE_CHANGE = (1u << 5),
};

enum conn_state_t {
    CONN_LIMBO = 0,
    CONN_CONNECTABLE,
    CONN_CONNECTABLE_AND_DISCOVERABLE,
    CONN_CONNECTED,
    CONN_OUTGOING_CALL,
    CONN_INCOMMING_CALL,
    CONN_ACTIVE_CALL,
    CONN_TEST_MODE,
    CONN_3WAY_WAITING,
    CONN_3WAY_HOLD,
    CONN_3WAY_MULTI,
    CONN_INCOMING_HOLD,
    CONN_ACTIVE_CALL2, // ???
    CONN_A2DP_STREAMING,
    CONN_LOW_BATTERY,
};

#define HFP_AUD_LVL_CHG (1u << 4)
#define HFP_MIC_LVL_CHG (1u << 5)

#define CONN_STATE_MASK (15u)

void RN52::enable(void)
{
    uint16_t enableTimeoutTick = xTaskGetTickCount() + powerUpTimeout;
    uart_config_t uart_config;

    if (this->_enabled)
        return;

    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 122;

    ESP_ERROR_CHECK(uart_set_pin(rn52_uart_num, rn52_tx_pin, rn52_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_param_config(rn52_uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_driver_install(rn52_uart_num, rn52_uart_buff_size, rn52_uart_buff_size, 10, &this->_uartQueue, 0));
    ESP_ERROR_CHECK(uart_flush(rn52_uart_num));

    this->initGpio();

    vTaskDelay(100 / portTICK_PERIOD_MS);
    this->setPower(true);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    this->setEnabled(true);

    printf("RN52: Enable...\n");
    vTaskDelay(10 / portTICK_PERIOD_MS);

    while (this->readLine() <= 0) {
        vTaskDelay(10 / portTICK_PERIOD_MS);
#ifdef DEBUG_INFINITE_ENABLE_TIMEOUT
        enableTimeoutTick = xTaskGetTickCount() + powerUpTimeout;
#endif
        if (enableTimeoutTick <= xTaskGetTickCount()) {
            printf("Failed to start RN52!!\n");
            this->setPower(false);
            return;
        }
    }
    printf("RN52: Init...\n");
    this->dumpCmd("V");

    this->setParam('N', "HEADUNIT");
    this->setParam('C', "200420");
    this->setParam('D', "04");
    this->setParam('K', "04");
    this->dumpCmd("D");
    this->connect();

    this->_lastStatus = 0;
    this->_isrCount = 0;
    this->_nextTick = xTaskGetTickCount();
    this->_enabled = true;
    this->_commandMode = true;
}

void RN52::loop(void)
{
    uint16_t status;
    bool commandMode = this->_commandMode;

    if (!this->_enabled)
        return;

    if (this->_isrCount == 0 && this->_nextTick > xTaskGetTickCount())
        return;
    this->_isrCount = 0;
    this->_nextTick = xTaskGetTickCount() + rn52_loop_delay;

    if (!commandMode)
        this->setCommand(true);

    if (this->getStatus(&status)) {
        this->_lastStatus = status;
        this->decodeStatus(status);
    }

    if (!commandMode)
        this->setCommand(false);
}

static std::map<std::string, MediaInfo::field_t> mediaInfoFields = {
    {"Title", MediaInfo::FIELD_TITLE},
    {"Artist", MediaInfo::FIELD_ARTIST},
    {"Album", MediaInfo::FIELD_ALBUM},
    {"TrackNumber", MediaInfo::FIELD_TRACK_NUM},
    {"TrackCount", MediaInfo::FIELD_TRACK_COUNT},
    {"Time(ms)", MediaInfo::FIELD_TIME_MS},
};

bool RN52::getMediaInfo(MediaInfo &mi)
{
    if (this->connState() != CONN_A2DP_STREAMING)
        return false;

    this->doCmd("AD", "AOK");
    while (this->readLine() > 0) {
        char *split = strchr(this->_line, '=');
        if (!split)
            continue;

        *split = '\0';
        std::string field(this->_line);
        std::string value(split + 1);

        auto iter = mediaInfoFields.find(field);
        if (iter != mediaInfoFields.end())
            mi.setField(iter->second, value);
    }
    return true;
}

void RN52::connect(void)
{
    this->doCmd("@,1");
    this->doCmd("B");
}

void RN52::disconnect(void)
{
    this->doCmd("K,4");
}

void RN52::eraseConnections(void)
{
    this->doCmd("U");
}

void RN52::trackNext(void)
{
    this->doCmd("AT+");
}

void RN52::trackPrev(void)
{
    this->doCmd("AT-");
}

void RN52::setStatusCallback(void (*cb)(event_t event, void *arg), void *arg)
{
    this->_statusCallback = cb;
    this->_statusCallbackArg = arg;
}

bool RN52::connected(void)
{
    return (this->connState() >= CONN_CONNECTED);
}

bool RN52::streaming(void)
{
    return (this->connState() == CONN_A2DP_STREAMING);
}

void RN52::initGpio(void)
{
    ESP_ERROR_CHECK(gpio_reset_pin(rn52_vdd_sw_pin));
    ESP_ERROR_CHECK(gpio_reset_pin(rn52_cmdn_pin));
    ESP_ERROR_CHECK(gpio_reset_pin(rn52_pwren_pin));
    ESP_ERROR_CHECK(gpio_reset_pin(rn52_frst_pin));
    ESP_ERROR_CHECK(gpio_reset_pin(rn52_int_pin));

    ESP_ERROR_CHECK(gpio_pullup_dis(rn52_vdd_sw_pin));
    ESP_ERROR_CHECK(gpio_pullup_dis(rn52_cmdn_pin));
    ESP_ERROR_CHECK(gpio_pullup_dis(rn52_pwren_pin));
    ESP_ERROR_CHECK(gpio_pullup_dis(rn52_frst_pin));
    ESP_ERROR_CHECK(gpio_pullup_dis(rn52_int_pin));

    ESP_ERROR_CHECK(gpio_set_level(rn52_vdd_sw_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(rn52_cmdn_pin, 0)); // Default to command mode!
    ESP_ERROR_CHECK(gpio_set_level(rn52_pwren_pin, 0));
    ESP_ERROR_CHECK(gpio_set_level(rn52_frst_pin, 0));

    ESP_ERROR_CHECK(gpio_set_direction(rn52_vdd_sw_pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(rn52_cmdn_pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(rn52_pwren_pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(rn52_frst_pin, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(rn52_int_pin, GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_intr_type(rn52_int_pin, GPIO_INTR_NEGEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(rn52_int_pin, RN52::intr, this));
}

void RN52::setPower(bool enabled)
{
    gpio_set_level(rn52_vdd_sw_pin, (enabled ? 1 : 0));
}

void RN52::setEnabled(bool enabled)
{
    gpio_set_level(rn52_pwren_pin, (enabled ? 1 : 0));
}

bool RN52::setCommand(bool enabled)
{
    if (enabled) {
        gpio_set_level(rn52_cmdn_pin, 0);
        return this->expect("CMD");
    } else {
        gpio_set_level(rn52_cmdn_pin, 1);
        return this->expect("END");
    }
}

bool RN52::setParam(char param, const char *value)
{
    if (this->getParam(param)) {
        if (strcmp(this->paramValue(), value) != 0) {
            snprintf(this->_line, sizeof(this->_line), "S%c,%s", param, value);
            return this->doCmd(this->_line, "AOK");
        } else {
            return true;
        }
    }
    return false;
}

bool RN52::getParam(char param)
{
    snprintf(this->_line, sizeof(this->_line), "G%c", param);
    return this->doCmd(this->_line, "");
}

const char *RN52::paramValue(void)
{
    if (this->_line[3] == ',')
        return &this->_line[4];
    return EMPTY_STR;
}

bool RN52::getStatus(uint16_t *status)
{
    uart_write_bytes(rn52_uart_num, "Q\r", 2);
    uart_wait_tx_done(rn52_uart_num, charTimeout);

    if (this->readLine() >= 4) {
        *status = strtoul(this->_line, NULL, 16);
        return true;
    }

    return false;
}

bool RN52::doCmd(const char *cmd, const char *expected)
{
    uart_write_bytes(rn52_uart_num, cmd, strlen(cmd));
    uart_write_bytes(rn52_uart_num, "\r", 1);
    uart_wait_tx_done(rn52_uart_num, charTimeout);

    return this->expect(expected);
}

bool RN52::dumpCmd(const char *cmd)
{
    bool result = false;
    uart_write_bytes(rn52_uart_num, cmd, strlen(cmd));
    uart_write_bytes(rn52_uart_num, "\r", 1);
    uart_wait_tx_done(rn52_uart_num, charTimeout);

    printf("%s?", cmd);
    while (this->readLine() > 0) {
        if (strcmp(this->_line, "AOK") == 0) {
            result = true;
            continue;
        }
        printf("RN52: %s\n", this->_line);
    }
    return result;
}

int RN52::readLine(void)
{
    bool cr, nl;
    size_t len;
    char *m = this->_line;

    cr = nl = false;

    do {
        len = uart_read_bytes(rn52_uart_num, m, 1, charTimeout);
        if (len > 0) {
            if (*m == '\r') {
                cr = true;
            } else if (cr) {
                if (*m == '\n') {
                    nl = true;
                    m--;
                    *m = '\0';
                    break;
                } else {
                    cr = false;
                }
            }
            m++;
        } else {
            return -1;
        }
    } while (!(cr && nl) && len > 0);

    return (m - this->_line);
}

bool RN52::expect(const char *expected)
{
    int expected_len = strlen(expected);
    return (this->readLine() >= expected_len && strncmp(this->_line, expected, expected_len) == 0);
}

void RN52::intr(void *arg)
{
    RN52 *self = static_cast<RN52*>(arg);
    self->_isrCount++;
    self->_nextTick = xTaskGetTickCount();
}

void RN52::decodeStatus(uint16_t status)
{
    unsigned conn_state, prev_conn_state;

    conn_state = status & CONN_STATE_MASK;
    prev_conn_state = this->_lastStatus & CONN_STATE_MASK;

#ifdef DEBUG_STATUS
    unsigned b[2];
    b[0] = (status >> 8) & 0xFF;
    b[1] = (status) & 0xFF;

    printf("RN52: status = %#04x,", status);
    if (b[0] & PROFILE_IAP)
        printf("PROFILE_IAP,");
    if (b[0] & PROFILE_SPP)
        printf("PROFILE_SPP,");
    if (b[0] & PROFILE_A2DP)
        printf("PROFILE_A2DP,");
    if (b[0] & PROFILE_HFP_HSP)
        printf("PROFILE_HFP_HSP,");
    if (b[0] & PROFILE_CID)
        printf("PROFILE_CID,");
    if (b[0] & PROFILE_CHANGE)
        printf("PROFILE_CHANGE,");

    if (b[1] & HFP_AUD_LVL_CHG)
        printf("HFP_AUD_LVL_CHG,");
    if (b[1] & HFP_MIC_LVL_CHG)
        printf("HFP_MIC_LVL_CHG,");

    printf("CONN_STATE=%d/%#2x ", conn_state, conn_state);
    switch (conn_state) {
        case CONN_LIMBO:
            printf("CONN_LIMBO\n");
            break;
        case CONN_CONNECTABLE:
            printf("CONN_CONNECTABLE\n");
            break;
        case CONN_CONNECTABLE_AND_DISCOVERABLE:
            printf("CONN_CONNECTABLE_AND_DISCOVERABLE\n");
            break;
        case CONN_CONNECTED:
            printf("CONN_CONNECTED\n");
            break;
        case CONN_OUTGOING_CALL:
            printf("CONN_OUTGOING_CALL\n");
            break;
        case CONN_INCOMMING_CALL:
            printf("CONN_INCOMMING_CALL\n");
            break;
        case CONN_ACTIVE_CALL:
            printf("CONN_ACTIVE_CALL\n");
            break;
        case CONN_TEST_MODE:
            printf("CONN_TEST_MODE\n");
            break;
        case CONN_3WAY_WAITING:
            printf("CONN_3WAY_WAITING\n");
            break;
        case CONN_3WAY_HOLD:
            printf("CONN_3WAY_HOLD\n");
            break;
        case CONN_3WAY_MULTI:
            printf("CONN_3WAY_MULTI\n");
            break;
        case CONN_INCOMING_HOLD:
            printf("CONN_INCOMING_HOLD\n");
            break;
        case CONN_ACTIVE_CALL2:
            printf("CONN_ACTIVE_CALL2\n");
            break;
        case CONN_A2DP_STREAMING:
            printf("CONN_A2DP_STREAMING\n");
            break;
        case CONN_LOW_BATTERY:
            printf("CONN_LOW_BATTERY\n");
            break;
        default:
            printf("INVALID???\n");
            break;
    }
#endif

    if (this->_statusCallback) {
        if (prev_conn_state != conn_state) {
            if (prev_conn_state == CONN_A2DP_STREAMING || conn_state == CONN_A2DP_STREAMING) {
                this->_statusCallback(RN52::EVENT_STREAMING, this->_statusCallbackArg);
            } else if ((prev_conn_state < CONN_CONNECTED && conn_state >= CONN_CONNECTED)
                || (prev_conn_state >= CONN_CONNECTED && conn_state < CONN_CONNECTED)) {
                this->_statusCallback(RN52::EVENT_CONNECTION, this->_statusCallbackArg);
            }
        }
    }
}

unsigned RN52::connState(void)
{
    return (this->_lastStatus & CONN_STATE_MASK);
}