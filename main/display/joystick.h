#pragma once
#include <stdio.h>
typedef enum
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN = 1,
    JOYSTICK_LEFT = 2,
    JOYSTICK_RIGHT = 3,
    JOYSTICK_BUTTON = 4
} joystick_event_t;

typedef struct
{
    uint8_t x_channel;
    uint8_t y_channel;
    uint8_t button;
    uint16_t threshold;
    void (*callback)(void*, joystick_event_t);
    void* callback_params;
} joystick_t;

typedef struct
{
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
    uint8_t button;
} joystick_state_t;

typedef struct
{
    joystick_t *joystick;
    joystick_state_t *state;
} read_joystick_task_params_t;

void init_joystick(read_joystick_task_params_t *joystick_params);