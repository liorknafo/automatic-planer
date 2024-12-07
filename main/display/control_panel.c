#include "menu.h"
#include "lcd1602.h"
#include "joystick.h"
#include "control_panel.h"
#include "driver/adc.h"

#define X_CHANNEL ADC1_CHANNEL_6
#define Y_CHANNEL ADC1_CHANNEL_5
#define BUTTON_JOYSTICK GPIO_NUM_4

#define I2C_MASTER_SCL_IO 22           // I²C clock pin
#define I2C_MASTER_SDA_IO 21           // I²C data pin
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000      // I²C frequency
#define LCD_NUM_ROWS 2
#define LCD_NUM_COLUMNS 16
#define LCD_ADDR 0x27                  // Adjust based on your I²C adapter address

void control_panel_task(void* pvParameters) {
    control_panel_params_t* params = (control_panel_params_t*)pvParameters;
    lcd_t lcd = {
        .addr = LCD_ADDR,
        .num_rows = LCD_NUM_ROWS,
        .num_columns = LCD_NUM_COLUMNS,
        .master_scl_io = I2C_MASTER_SCL_IO,
        .master_sda_io = I2C_MASTER_SDA_IO,
        .master_num = I2C_MASTER_NUM,
        .master_freq_hz = I2C_MASTER_FREQ_HZ
    };
    i2c_master_init(&lcd);
    lcd_init(&lcd);
    joystick_t joystick = {
        .x_channel = X_CHANNEL,
        .y_channel = Y_CHANNEL,
        .button = BUTTON_JOYSTICK,
        .threshold = 600,
        .callback = NULL,
        .callback_params = NULL
    };
    joystick_state_t state = {
        .up = false,
        .down = false,
        .left = false,
        .right = false,
        .button = false
    };
    read_joystick_task_params_t joystick_params = {
        .joystick = &joystick,
        .state = &state
    };
    init_joystick(&joystick_params);
    menu_item_t items[] = {
        {"Cut Speed", &params->cutting_speed_mm_sec, 0.1, 0.1, 10},
        {"Trv Speed", &params->travel_speed_mm_sec, 0.1, 0.1, 10},
        {"Y Offset", &params->y_offset_mm, 1, 0, 100}
    };
    menu_t menu = {
        .lcd = &lcd,
        .joystick = &joystick,
        .items = items,
        .num_items = sizeof(items) / sizeof(menu_item_t),
        .max_displayed_items = 2
    };
    menu_task_params_t menu_task_params = {
        .menu = &menu,
        .scroll = 0,
        .selected = 0
    };
    init_menu(&menu_task_params);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}