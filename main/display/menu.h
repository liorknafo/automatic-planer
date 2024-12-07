#pragma once
#include "lcd1602.h"
#include "joystick.h"

typedef struct
{
    char *name;
    float *value;
    float increment;
    float min;
    float max;
} menu_item_t;

typedef struct
{
    lcd_t *lcd;
    joystick_t *joystick;
    menu_item_t *items;
    int num_items;
    int max_displayed_items;
} menu_t;

typedef struct
{
    menu_t *menu;
    int scroll;
    int selected;
} menu_task_params_t;

void init_menu(menu_task_params_t* menu_task_params);