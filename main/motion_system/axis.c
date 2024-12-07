#include "motion_system/motor.h"
#include "motion_system/micro_switch.h"
#include "freertos/FreeRTOS.h"

typedef struct
{
    motor_t *motor;
    uint8_t *start_switches;
    uint8_t *end_switches;
    uint8_t num_motors;
    float mm_per_step;
} axis_t;

void move_to_switches(axis_t *axis, motor_dir_t dir, float speed_mm_sec)
{
    float steps_per_second = speed_mm_sec / axis->mm_per_step;
    int delay_period_ms = (int)(1000 / steps_per_second) - PULSE_WIDTH_MS;
    uint8_t *switches = dir == MOTOR_FORWARD ? axis->end_switches : axis->start_switches;
    for (int i = 0; i < axis->num_motors; i++)
    {
        set_dir(axis->motor + i, dir);
    }
    uint8_t moving = 1;
    while (moving)
    {
        moving = 1;
        for (int i = 0; i < axis->num_motors; i++)
        {
            if (micro_switch_pressed(switches[i]))
            {
                moving = 0;
            }
        }
        if (moving)
        {
            for (int i = 0; i < axis->num_motors; i++)
            {
                send_pulse(axis->motor + i);
            }
            vTaskDelay(pdMS_TO_TICKS(delay_period_ms));
        }
    }
}