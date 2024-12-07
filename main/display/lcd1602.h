#pragma once
#include "driver/i2c.h"
typedef struct
{
    uint8_t addr;
    uint8_t num_rows;
    uint8_t num_columns;
    uint8_t master_scl_io;
    uint8_t master_sda_io;
    uint8_t master_num;
    uint32_t master_freq_hz;
} lcd_t;

esp_err_t i2c_master_init(lcd_t* lcd);
void lcd_init (lcd_t* lcd);
void lcd_put_cur(lcd_t* lcd, int row, int col);
void lcd_send_string (lcd_t* lcd, char *str);
void lcd_clear (lcd_t* lcd);
