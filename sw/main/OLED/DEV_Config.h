#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <stdint.h>

#define USE_IMAGES 0

#define OLED_CMD        0
#define OLED_RAM        1

void OLED_Sys_Init(void);
void OLED_Reset(void);
void OLED_WriteReg(uint8_t reg);
void OLED_WriteRegBlock(uint8_t *data, uint16_t len);
void OLED_WriteData(uint8_t data);
void OLED_WriteDataBlock(uint8_t *data, uint16_t len);
void OLED_Delay_ms(uint32_t ms);

#endif
