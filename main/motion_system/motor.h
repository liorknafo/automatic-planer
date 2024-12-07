#pragma once
#include <stdint.h>
#define PULSE_WIDTH_MS 1

typedef enum {
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD = 1
} motor_dir_t;

typedef struct {
    uint8_t pulse_pin;
    uint8_t dir_pin;
    uint8_t en_pin;
    uint8_t invert_dir;
} motor_t;

void init_motor(motor_t* motor);
void set_dir(motor_t *motor, motor_dir_t dir);
void send_pulse(motor_t* motor);