#ifndef LCD5110_H
#define LCD5110_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "gpio.h"
#include "stm32l1xx_hal_spi.h"


uint8_t display_buffer[6][84];


typedef struct {
    uint8_t * buf;
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef * RESET_BASE;
    uint16_t RESET_PIN;
    GPIO_TypeDef * COM_DAT_BASE;
    uint16_t COM_DAT_PIN;
    GPIO_TypeDef * SCE_BASE;
    uint16_t SCE_PIN;
} display_5110;

int32_t init_display (display_5110 * display);

int32_t set_position (display_5110 * display, uint8_t x, uint8_t y);

int32_t print_char (display_5110 * display, char *chr);

int32_t print_string (display_5110 * display, char * buf, uint32_t size);

int32_t refresh_display (display_5110 * display);

int32_t draw_line();

int32_t draw_rect();

int32_t draw_circus();

int32_t draw_triangle();

int32_t move_picture();

int32_t clear_area(display_5110 * display);

int32_t clear_display (display_5110 * display);
#endif // LCD5110_H
