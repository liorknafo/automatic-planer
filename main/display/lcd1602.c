#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "lcd1602.h"
             // Adjust based on your I²C adapter address
#define TAG "LCD1602"


esp_err_t i2c_master_init(lcd_t* lcd)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = lcd->master_sda_io,
        .scl_io_num = lcd->master_scl_io,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = lcd->master_freq_hz,
    };

    i2c_param_config(lcd->master_num, &conf);

    return i2c_driver_install(lcd->master_num, conf.mode, 0, 0, 0);
}

void lcd_send_cmd (lcd_t* lcd, char cmd)
{
	esp_err_t err;
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0

	err= i2c_master_write_to_device (lcd->master_num, lcd->addr, data_t, 4, 1000);
	if (err != 0) ESP_LOGI(TAG, "Error no. %d in command", err);
}

void lcd_init (lcd_t* lcd)
{
	// 4 bit initialisation
	vTaskDelay(pdMS_TO_TICKS(50));  // wait for >40ms
	lcd_send_cmd (lcd, 0x30);
	esp_rom_delay_us(4500);  // wait for >4.1ms
	lcd_send_cmd (lcd, 0x30);
	esp_rom_delay_us(200);  // wait for >100us
	lcd_send_cmd (lcd, 0x30);
	esp_rom_delay_us(200);
	lcd_send_cmd (lcd, 0x20);  // 4bit mode
	esp_rom_delay_us(200);

    // dislay initialisation
	lcd_send_cmd (lcd, 0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	vTaskDelay(pdTICKS_TO_MS(1));
	lcd_send_cmd (lcd, 0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	vTaskDelay(pdTICKS_TO_MS(1));
	lcd_send_cmd (lcd, 0x01);  // clear display
	vTaskDelay(pdTICKS_TO_MS(2));
	lcd_send_cmd (lcd, 0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	vTaskDelay(pdTICKS_TO_MS(1));
	lcd_send_cmd (lcd, 0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	vTaskDelay(pdTICKS_TO_MS(2));
}

void lcd_put_cur(lcd_t* lcd, int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (lcd, col);
}

void lcd_send_data (lcd_t* lcd, char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	int err = i2c_master_write_to_device(lcd->master_num, lcd->addr, data_t, 4, 1000);
	if (err!=0) ESP_LOGI(TAG, "Error in sending data");
}

void lcd_send_string (lcd_t* lcd, char *str)
{
	while (*str) lcd_send_data (lcd, *str++);
}

void lcd_clear (lcd_t* lcd)
{
	lcd_send_cmd (lcd, 0x01);
	vTaskDelay(pdTICKS_TO_MS(5));
}