#pragma once
#include <stdint.h>
#include "motion_system/motor.h"

typedef struct
{
    motor_t *motor;
    uint8_t *start_switches;
    uint8_t *end_switches;
    uint8_t num_motors;
    float mm_per_step;
} axis_t;

void move_to_switches(axis_t *axis, motor_dir_t dir, float speed_mm_sec);