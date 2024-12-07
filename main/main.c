// main.c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/adc.h"

#define STEP_Y_SWITCH_INCREMENT_GPIO GPIO_NUM_0
#define STEP_Y_DIR_GPIO GPIO_NUM_27
#define STEP_Y_EN_GPIO GPIO_NUM_15
#define STEP_Y_PULSE_GPIO GPIO_NUM_25

#define STEP_Y_SWITCH_START_LIMIT_GPIO GPIO_NUM_18 
#define STEP_Y_SWITCH_END_LIMIT_GPIO GPIO_NUM_35 

#define STEP_X_RIGHT_DIR_GPIO GPIO_NUM_0
#define STEP_X_RIGHT_EN_GPIO GPIO_NUM_4
#define STEP_X_RIGHT_PULSE_GPIO GPIO_NUM_5
#define STEP_X_LEFT_DIR_GPIO GPIO_NUM_13
#define STEP_X_LEFT_EN_GPIO GPIO_NUM_14
#define STEP_X_LEFT_PULSE_GPIO GPIO_NUM_15

#define STEP_X_SWITCH_START_LIMIT_RIGHT_GPIO GPIO_NUM_17
#define STEP_X_SWITCH_START_LIMIT_LEFT_GPIO GPIO_NUM_18
#define STEP_X_SWITCH_END_LIMIT_RIGHT_GPIO GPIO_NUM_19
#define STEP_X_SWITCH_END_LIMIT_LEFT_GPIO GPIO_NUM_21

#define SWITCH_INCREMENT_PULLY_TEETH 37
#define SWITCH_INCREMENT_TEETH_COUNT 15
#define PULSE_WIDTH_MS 1
#define MOTOR_STEPS 200
#define PULLY_TEETH 16
#define DISTANCE_PER_TEETH 2
#define STEP_Y_SWITCH_DISTANCE SWITCH_INCREMENT_PULLY_TEETH *DISTANCE_PER_TEETH / SWITCH_INCREMENT_TEETH_COUNT
#define MICROSTEPS 1
#define MM_PER_STEP ((float)DISTANCE_PER_TEETH * PULLY_TEETH / MOTOR_STEPS / MICROSTEPS)

#define START_BUTTON_GPIO GPIO_NUM_22
#define STOP_BUTTON_GPIO GPIO_NUM_23

float cutting_speed_mm_sec = 1.0;
float travel_speed_mm_sec = 5.0;

float y_offset_mm = 50;
float y_step_count = 0;

// void xHome()
// {
//     gpio_set_level(STEP_X_LEFT_DIR_GPIO, 0);
//     gpio_set_level(STEP_X_RIGHT_DIR_GPIO, 0);
//     gpio_set_level(STEP_X_LEFT_EN_GPIO, 0);
//     gpio_set_level(STEP_X_RIGHT_EN_GPIO, 0);
//     gpio_set_level(STEP_X_LEFT_PULSE_GPIO, 0);
//     gpio_set_level(STEP_X_RIGHT_PULSE_GPIO, 0);

//     while (gpio_get_level(STEP_X_SWITCH_START_LIMIT_LEFT_GPIO) == 0 || gpio_get_level(STEP_X_SWITCH_START_LIMIT_RIGHT_GPIO) == 0)
//     {
//         if (gpio_get_level(STEP_X_SWITCH_START_LIMIT_LEFT_GPIO) == 0)
//         {
//             gpio_set_level(STEP_X_LEFT_PULSE_GPIO, 1);
//         }
//         if (gpio_get_level(STEP_X_SWITCH_START_LIMIT_RIGHT_GPIO) == 0)
//         {
//             gpio_set_level(STEP_X_RIGHT_PULSE_GPIO, 1);
//         }
//         vTaskDelay(pdMS_TO_TICKS(PULSE_WIDTH_MS));
//         if (gpio_get_level(STEP_X_SWITCH_START_LIMIT_LEFT_GPIO) == 0)
//         {
//             gpio_set_level(STEP_X_LEFT_PULSE_GPIO, 0);
//         }
//         if (gpio_get_level(STEP_X_SWITCH_START_LIMIT_RIGHT_GPIO) == 0)
//         {
//             gpio_set_level(STEP_X_RIGHT_PULSE_GPIO, 0);
//         }
//         vTaskDelay(((1000 * MM_PER_STEP / travel_speed_mm_sec) - PULSE_WIDTH_MS) / portTICK_PERIOD_MS);
//     }
// }

// void yHome()
// {
//     gpio_set_level(STEP_Y_DIR_GPIO, 0);
//     gpio_set_level(STEP_Y_EN_GPIO, 0);
//     gpio_set_level(STEP_Y_PULSE_GPIO, 0);

//     while (gpio_get_level(STEP_Y_SWITCH_START_LIMIT_GPIO) == 0)
//     {
//         gpio_set_level(STEP_Y_PULSE_GPIO, 1);
//         vTaskDelay(pdMS_TO_TICKS(PULSE_WIDTH_MS));
//         gpio_set_level(STEP_Y_PULSE_GPIO, 0);
//         vTaskDelay(((1000 * MM_PER_STEP / travel_speed_mm_sec) - PULSE_WIDTH_MS) / portTICK_PERIOD_MS);
//     }
// }

// void moveYToEdge(bool direction, float speed_mm_sec)
// {
//     gpio_set_level(STEP_Y_DIR_GPIO, direction);
//     gpio_set_level(STEP_Y_EN_GPIO, 0);
//     gpio_set_level(STEP_Y_PULSE_GPIO, 0);
//     while (gpio_get_level(STEP_Y_SWITCH_END_LIMIT_GPIO) == 0)
//     {
//         gpio_set_level(STEP_Y_PULSE_GPIO, 1);
//         vTaskDelay(pdMS_TO_TICKS(PULSE_WIDTH_MS));
//         gpio_set_level(STEP_Y_PULSE_GPIO, 0);
//         vTaskDelay(((1000 * MM_PER_STEP / speed_mm_sec) - PULSE_WIDTH_MS) / portTICK_PERIOD_MS);
//     }
// }

// void moveXOffset(bool direction, float speed_mm_sec)
// {
//     int start_steps = y_step_count;
//     int end_steps = y_offset_mm / MM_PER_STEP;
//     gpio_set_level(STEP_X_LEFT_DIR_GPIO, direction);
//     gpio_set_level(STEP_X_RIGHT_DIR_GPIO, direction);
    
//     while (
//         gpio_get_level(STEP_X_SWITCH_END_LIMIT_LEFT_GPIO) == 0 && 
//         gpio_get_level(STEP_X_SWITCH_END_LIMIT_RIGHT_GPIO) == 0 && 
//         abs(y_step_count - start_steps) >= end_steps)
//     {
//         gpio_set_level(STEP_X_LEFT_PULSE_GPIO, 1);
//         gpio_set_level(STEP_X_RIGHT_PULSE_GPIO, 1);
//         vTaskDelay(pdMS_TO_TICKS(PULSE_WIDTH_MS));
//         gpio_set_level(STEP_X_LEFT_PULSE_GPIO, 0);
//         gpio_set_level(STEP_X_RIGHT_PULSE_GPIO, 0);
//         vTaskDelay(((1000 * MM_PER_STEP / speed_mm_sec) - PULSE_WIDTH_MS) / portTICK_PERIOD_MS);
//     }
// }

// TaskHandle_t main_sequence_task = NULL;
// void main_sequence(void *pvParameters)
// {
//     yHome();
//     xHome();

//     while (gpio_get_level(STEP_Y_SWITCH_END_LIMIT_GPIO) == 0) {
//         moveYToEdge(1, cutting_speed_mm_sec);
//         moveYToEdge(0, travel_speed_mm_sec);
//         moveXOffset(1, cutting_speed_mm_sec);
//     }
// }

// void setup()
// {
//     // gpio_set_direction(JOYSTICK_BUTTON_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(JOYSTICK_X_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(JOYSTICK_Y_GPIO, GPIO_MODE_INPUT);
//     adc1_config_width(ADC_WIDTH_BIT_12);
//     adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_12);

//     // gpio_set_direction(STEP_Y_SWITCH_INCREMENT_GPIO, GPIO_MODE_INPUT);

//     // gpio_set_direction(STEP_Y_SWITCH_START_LIMIT_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(STEP_Y_SWITCH_END_LIMIT_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(STEP_X_SWITCH_START_LIMIT_RIGHT_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(STEP_X_SWITCH_START_LIMIT_LEFT_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(STEP_X_SWITCH_END_LIMIT_RIGHT_GPIO, GPIO_MODE_INPUT);
//     // gpio_set_direction(STEP_X_SWITCH_END_LIMIT_LEFT_GPIO, GPIO_MODE_INPUT);

//     // gpio_set_direction(STEP_Y_DIR_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_Y_EN_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_Y_PULSE_GPIO, GPIO_MODE_OUTPUT);

//     // gpio_set_direction(STEP_X_LEFT_DIR_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_X_LEFT_EN_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_X_LEFT_PULSE_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_X_RIGHT_DIR_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_X_RIGHT_EN_GPIO, GPIO_MODE_OUTPUT);
//     // gpio_set_direction(STEP_X_RIGHT_PULSE_GPIO, GPIO_MODE_OUTPUT);
// }

// void app_main(void)
// {
//     setup();
//     while (true)
//     {
//         if (gpio_get_level(START_BUTTON_GPIO) == 0 && main_sequence_task == NULL)
//         {
//             xTaskCreate(main_sequence, "main_sequence", 4096, NULL, 5, &main_sequence_task);
//         }
//         else if (gpio_get_level(STOP_BUTTON_GPIO) == 0 && main_sequence_task != NULL)
//         {
//             vTaskDelete(main_sequence_task);
//             main_sequence_task = NULL;
//         }
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }

// #define NO_OF_SAMPLES 64
// #define X_CHANNEL ADC1_CHANNEL_6
// #define Y_CHANNEL ADC1_CHANNEL_5
// #define ADC_WIDTH ADC_WIDTH_BIT_12
// #define ADC_ATTEN ADC_ATTEN_DB_12
// #define BUTTON_JOYSTICK GPIO_NUM_4

// void app_main() {
//     // Configure ADC1 width and attenuation
//     adc1_config_width(ADC_WIDTH_BIT_12); // 12-bit resolution
//     adc1_config_channel_atten(X_CHANNEL, ADC_ATTEN_DB_12); // GPIO34
//     adc1_config_channel_atten(Y_CHANNEL, ADC_ATTEN_DB_12); // GPIO33
//     gpio_set_direction(BUTTON_JOYSTICK, GPIO_MODE_INPUT);
//     gpio_set_pull_mode(BUTTON_JOYSTICK, GPIO_PULLUP_ONLY);

//     while (1) {
//         int adc_reading = adc1_get_raw(X_CHANNEL);
//         printf("Channel X Reading: %d\n", adc_reading);
//         adc_reading = adc1_get_raw(Y_CHANNEL);
//         printf("Channel Y Reading: %d\n", adc_reading);
//         int botton_reading = gpio_get_level(JOYSTICK_BUTTON_GPIO);
//         printf("Botton Reading: %d\n", botton_reading);
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
//     }
// }

// #include "lcd1602.h"

// #define I2C_MASTER_SCL_IO 22           // I²C clock pin
// #define I2C_MASTER_SDA_IO 21           // I²C data pin
// #define I2C_MASTER_NUM I2C_NUM_0
// #define I2C_MASTER_FREQ_HZ 100000      // I²C frequency
// #define LCD_NUM_ROWS 2
// #define LCD_NUM_COLUMNS 16
// #define LCD_ADDR 0x27                  // Adjust based on your I²C adapter address

// void app_main(void)
// {
//     lcd_t lcd = {
//         .addr = LCD_ADDR,
//         .num_rows = LCD_NUM_ROWS,
//         .num_columns = LCD_NUM_COLUMNS,
//         .master_scl_io = I2C_MASTER_SCL_IO,
//         .master_sda_io = I2C_MASTER_SDA_IO,
//         .master_num = I2C_MASTER_NUM,
//         .master_freq_hz = I2C_MASTER_FREQ_HZ
//     };
//     ESP_ERROR_CHECK(i2c_master_init(&lcd));

//     lcd_init(&lcd);
//     lcd_clear(&lcd);

//     lcd_put_cur(&lcd, 0, 0);
//     lcd_send_string(&lcd, "Hello World!");

//     lcd_put_cur(&lcd, 1, 0);
//     lcd_send_string(&lcd, "from ESP32");
// }

#include "display/control_panel.h"
#include "motion_system/axis.h"


void app_main() {
    control_panel_params_t control_panel_params = {
        .cutting_speed_mm_sec = cutting_speed_mm_sec,
        .travel_speed_mm_sec = travel_speed_mm_sec,
        .y_offset_mm = y_offset_mm
    };
    // control_panel_task(&control_panel_params);
    xTaskCreate(control_panel_task, "control_panel_task", 8096, &control_panel_params, 5, NULL);
    motor_t motor = {
        .pulse_pin = STEP_Y_PULSE_GPIO,
        .dir_pin = STEP_Y_DIR_GPIO,
        .en_pin = STEP_Y_EN_GPIO,
        .invert_dir = 0
    };
    init_motor(&motor);
    uint8_t start_switches[] = {STEP_Y_SWITCH_START_LIMIT_GPIO};
    uint8_t end_switches[] = {STEP_Y_SWITCH_END_LIMIT_GPIO};
    axis_t axis = {
        .motor = &motor,
        .start_switches = start_switches,
        .end_switches = end_switches,
        .num_motors = 1,
        .mm_per_step = MM_PER_STEP
    };
    move_to_switches(&axis, MOTOR_FORWARD , cutting_speed_mm_sec);
}