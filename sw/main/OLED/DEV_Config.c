#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <freertos/task.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DEV_Config.h"

//OLED GPIO
static const gpio_num_t DISP_MOSI = GPIO_NUM_23;
static const gpio_num_t DISP_SCLK = GPIO_NUM_18;
static const gpio_num_t DISP_DC = GPIO_NUM_4; // HACK: Relocated from 2 due to USB programming interference
static const gpio_num_t DISP_RST = GPIO_NUM_15;
static const gpio_num_t DISP_CS = GPIO_NUM_5;
static const spi_host_device_t DISP_SPI_BUS = SPI3_HOST;

static const spi_bus_config_t bus_config = {
    .miso_io_num = -1,
    .mosi_io_num = DISP_MOSI,
    .sclk_io_num = DISP_SCLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 128*128/2, // Worst case, entire buffer ram
};

static const spi_device_interface_config_t dev_config = {
    .mode = 0,
    .clock_speed_hz = SPI_MASTER_FREQ_20M,
    .spics_io_num = DISP_CS,
    .queue_size=16,
};

static spi_device_handle_t spi_handle;

void OLED_Sys_Init(void)
{
    // init gpio
    ESP_ERROR_CHECK(gpio_reset_pin(DISP_DC));
    ESP_ERROR_CHECK(gpio_reset_pin(DISP_RST));

    ESP_ERROR_CHECK(gpio_pullup_dis(DISP_DC));
    ESP_ERROR_CHECK(gpio_pullup_dis(DISP_RST));

    ESP_ERROR_CHECK(gpio_set_level(DISP_DC, 0));
    ESP_ERROR_CHECK(gpio_set_level(DISP_RST, 0));

    ESP_ERROR_CHECK(gpio_set_direction(DISP_DC, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DISP_RST, GPIO_MODE_OUTPUT));

    // init spi module
    ESP_ERROR_CHECK(spi_bus_initialize(DISP_SPI_BUS, &bus_config, DISP_SPI_BUS));
    ESP_ERROR_CHECK(spi_bus_add_device(DISP_SPI_BUS, &dev_config, &spi_handle));
}

static void OLED_Write_Byte(uint8_t value, uint8_t Cmd)
{
    spi_transaction_t desc = {
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = { value },
        .length = 8,
    };
    gpio_set_level(DISP_DC, Cmd);
    spi_device_transmit(spi_handle, &desc);
}

static void OLED_Write_Block(uint8_t *data, uint16_t len, uint8_t Cmd)
{
    spi_transaction_t desc = {
        .flags = 0,
        .tx_buffer = data,
        .length = len * 8,
    };
    gpio_set_level(DISP_DC, Cmd);
    spi_device_transmit(spi_handle, &desc);
}

void OLED_Reset(void)
{
    gpio_set_level(DISP_RST, 1);
    OLED_Delay_ms(100);
    gpio_set_level(DISP_RST, 0);
    OLED_Delay_ms(100);
    gpio_set_level(DISP_RST, 1);
    OLED_Delay_ms(100);
}

void OLED_WriteReg(uint8_t reg)
{
    OLED_Write_Byte(reg, OLED_CMD);
}

void OLED_WriteData(uint8_t data)
{
    OLED_Write_Byte(data, OLED_RAM);
}

void OLED_WriteDataBlock(uint8_t *data, uint16_t len)
{
    OLED_Write_Block(data, len, OLED_RAM);
}

void OLED_Delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}