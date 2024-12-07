#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "motion_system/motor.h"

void init_motor(motor_t *motor)
{
    gpio_set_direction(motor->pulse_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(motor->dir_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction(motor->en_pin, GPIO_MODE_OUTPUT);
}

motor_dir_t invert_dir(motor_dir_t dir)
{
    return dir == MOTOR_FORWARD ? MOTOR_BACKWARD : MOTOR_FORWARD;
}

void set_dir(motor_t *motor, motor_dir_t dir)
{
    printf("Setting direction to %d with dir pin %d\n", motor->invert_dir ? invert_dir(dir) : dir, motor->dir_pin);
    gpio_set_level(motor->dir_pin, motor->invert_dir ? invert_dir(dir) : dir);
}

void send_pulse(motor_t *motor)
{
    gpio_set_level(motor->pulse_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(PULSE_WIDTH_MS));
    gpio_set_level(motor->pulse_pin, 0);
}