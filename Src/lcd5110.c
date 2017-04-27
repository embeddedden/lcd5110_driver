#include "lcd5110.h"
#include "gpio.h"
#include "spi.h"
#include "ascii_table.h"

static void Soft_Delay(volatile uint32_t number)
{
        while(number--);
}

static void switch_to_command_mode (display_5110 *display)
{
    HAL_GPIO_WritePin(display->COM_DAT_BASE, display->COM_DAT_PIN, GPIO_PIN_RESET);
}

static void switch_to_data_mode (display_5110 *display)
{
    HAL_GPIO_WritePin(display->COM_DAT_BASE, display->COM_DAT_PIN, GPIO_PIN_SET);
}

int32_t init_display(display_5110 *display)
{
    uint8_t byte = 0;
    HAL_GPIO_WritePin(display->RESET_BASE, display->RESET_PIN, GPIO_PIN_SET); //reset disable

    Soft_Delay(0xFF);
    HAL_GPIO_WritePin(display->RESET_BASE, display->RESET_PIN, GPIO_PIN_RESET); //reset enable
    Soft_Delay(0xFFFF);
    HAL_GPIO_WritePin(display->RESET_BASE, display->RESET_PIN, GPIO_PIN_SET); //reset disable

    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET); //chip enable
    Soft_Delay(0xFFFFF);
    HAL_GPIO_WritePin(display->COM_DAT_BASE, display->COM_DAT_PIN, GPIO_PIN_RESET); //commands

    Soft_Delay(0xFFFF);
    byte = 0x21;
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    Soft_Delay(0xFFFF);
    //Bias System
    byte = 0x13;
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);

    Soft_Delay(0xFFFF);
    //temp coef
    byte = 0x04;
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);

    Soft_Delay(0xFFFF);
    //Vop
    byte = 0xB8;
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);

    Soft_Delay(0xFFFF);

    byte = 0x20;
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);

    Soft_Delay(0xFFFF);
//    byte = 0x08;
//    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
//    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);
//    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    byte = 0x0C;
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &byte, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);

    Soft_Delay(0xFFFF);

    HAL_GPIO_WritePin(display->COM_DAT_BASE, display->COM_DAT_PIN, GPIO_PIN_SET); //data

    return 0;
}



int32_t print_char(display_5110 *display, char *chr)
{
    switch_to_data_mode(display);
    uint8_t * byte = small_ascii_font + ((uint8_t)*chr - 0x20)*6;
    for(int i = 0; i < 6; i++)
    {
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, byte+i, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    }
    return 0;
}


int32_t set_position (display_5110 *display, uint8_t x, uint8_t y)
{
    if (!display)
        return -1;
    if (x > 83 || y > 5)
        return -1;
    switch_to_command_mode(display);
    uint8_t byteY = 0x40 + y;
    uint8_t byteX = 0x80 + x;
    //set y address
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &byteY, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    //set X address
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, &byteX, 1, 100);
    HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    switch_to_data_mode(display);
    return 0;
}


int32_t clear_display(display_5110 *display)
{
    uint8_t byte = 0x00;
    set_position(display, 0, 0);
    for(uint32_t i = 0; i < 6*84; i++)
    {
        HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi3, &byte, 1, 100);
        HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    }
}

int32_t print_string(display_5110 *display, char *buf, uint32_t size)
{
    for(uint32_t i = 0; i < size; i++)
    {
        print_char(display, buf+i);
    }
    return 0;
}

int32_t refresh_display(display_5110 *display)
{
    switch_to_data_mode(display);
    for(int j = 0; j < 6; j++)
    {
    set_position(display, 0, j);
    for(int i = 0; i < 84; i++)
    {
        HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_RESET);
        HAL_SPI_Transmit(display->hspi, &display->buf[j*84 + i], 1, 10);
        HAL_GPIO_WritePin(display->SCE_BASE, display->SCE_PIN, GPIO_PIN_SET);
    }
    }
    return 0;
}
