#include "joystick.h"
#include "lcd1602.h"
#include "menu.h"
#include "freertos/FreeRTOS.h"

void joystick_callback(void *param, joystick_event_t event)
{
    menu_task_params_t *menu_task_params = (menu_t *)param;
    float inc, min_val, max_val;
    float* value;

    switch (event)
    {
    case JOYSTICK_UP:
        menu_task_params->selected--;
        if (menu_task_params->selected < 0)
        {
            menu_task_params->selected = 0;
        }
        break;
    case JOYSTICK_DOWN:
        menu_task_params->selected++;
        if (menu_task_params->selected >= menu_task_params->menu->num_items)
        {
            menu_task_params->selected = menu_task_params->menu->num_items - 1;
        }
        break;
    case JOYSTICK_LEFT:
        value = menu_task_params->menu->items[menu_task_params->selected].value;
        inc = menu_task_params->menu->items[menu_task_params->selected].increment;
        min_val = menu_task_params->menu->items[menu_task_params->selected].min;
        if ((*value) - inc < min_val) {
            *value = min_val;
        } else {
            *value -= inc;
        } 
        break;
    case JOYSTICK_RIGHT:
        value = menu_task_params->menu->items[menu_task_params->selected].value;
        inc = menu_task_params->menu->items[menu_task_params->selected].increment;
        max_val = menu_task_params->menu->items[menu_task_params->selected].max;
        if ((*value) + inc > max_val) {
            *value = max_val;
        } else {
            *value += inc;
        }
        break;
    case JOYSTICK_BUTTON:
        break;
    }

    if (menu_task_params->scroll > menu_task_params->selected)
    {
        menu_task_params->scroll = menu_task_params->selected;
    }
    else if (menu_task_params->scroll < menu_task_params->selected - menu_task_params->menu->max_displayed_items + 1)
    {
        menu_task_params->scroll = menu_task_params->selected - menu_task_params->menu->max_displayed_items + 1;
    }
}

void menu_task(void *param)
{
    menu_task_params_t *menu_task_params = (menu_task_params_t *)param;
    menu_t *menu = menu_task_params->menu;
    lcd_t *lcd = menu_task_params->menu->lcd;
    joystick_t *joystick = menu->joystick;
    menu_item_t *items = menu->items;
    int num_items = menu_task_params->menu->num_items;
    
    while (1)
    {
        int scroll = menu_task_params->scroll;
        int selected = menu_task_params->selected;
        // lcd_clear(lcd);
        for (int i = 0; i < menu->max_displayed_items; i++) {
            if (scroll + i == selected) {
                lcd_put_cur(lcd, i, 0);
                lcd_send_string(lcd, ">");
            } else {
                lcd_put_cur(lcd, i, 0);
                lcd_send_string(lcd, " ");
            }
            
            lcd_put_cur(lcd, i, 1);
            lcd_send_string(lcd, items[scroll + i].name);
            
            lcd_put_cur(lcd, i, 11);
            char buffer[6];
            sprintf(buffer, "%0.1f", *items[scroll + i].value);
            lcd_send_string(lcd, buffer);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void init_menu(menu_task_params_t* menu_task_params)
{
    menu_t *menu = menu_task_params->menu;
    joystick_t* joystick = menu->joystick;
    joystick->callback = joystick_callback;
    joystick->callback_params = menu_task_params;
    xTaskCreate(menu_task, "menu_task", 4096, menu_task_params, 5, NULL);
}