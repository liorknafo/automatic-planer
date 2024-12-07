#include "driver/adc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "joystick.h"

#define ADC_WIDTH ADC_WIDTH_BIT_12
#define ADC_ATTEN ADC_ATTEN_DB_12

void read_joystick_task(void *pvParameters)
{
    read_joystick_task_params_t *params = (read_joystick_task_params_t *)pvParameters;
    joystick_t *joystick = params->joystick;
    joystick_state_t *state = params->state;
    while (1)
    {
        int x = (int)adc1_get_raw(joystick->x_channel) - 2048;
        int y = (int)adc1_get_raw(joystick->y_channel) - 2048;
        int button = gpio_get_level(joystick->button);
        if (y > joystick->threshold && !state->up)
        {
            if (joystick->callback != NULL)
                joystick->callback(joystick->callback_params, JOYSTICK_UP);
            state->up = true;
        }
        else if (y < joystick->threshold)
        {
            state->up = false;
        }

        if (y < -joystick->threshold && !state->down)
        {
            if (joystick->callback != NULL)
                joystick->callback(joystick->callback_params, JOYSTICK_DOWN);
            state->down = true;
        }
        else if (y > -joystick->threshold)
        {
            state->down = false;
        }

        if (x > joystick->threshold && !state->right)
        {
            if (joystick->callback != NULL)
                joystick->callback(joystick->callback_params, JOYSTICK_RIGHT);
            state->right = true;
        }
        else if (x < joystick->threshold)
        {
            state->right = false;
        }

        if (x < -joystick->threshold && !state->left)
        {
            if (joystick->callback != NULL)
                joystick->callback(joystick->callback_params, JOYSTICK_LEFT);
            state->left = true;
        }
        else if (x > -joystick->threshold)
        {
            state->left = false;
        }

        if (button == 0 && !state->button)
        {
            if (joystick->callback != NULL)
                joystick->callback(joystick->callback_params, JOYSTICK_BUTTON);
            state->button = true;
        }
        else if (button == 1)
        {
            state->button = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void init_joystick(read_joystick_task_params_t *joystick_params)
{
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(joystick_params->joystick->x_channel, ADC_ATTEN);
    adc1_config_channel_atten(joystick_params->joystick->y_channel, ADC_ATTEN);
    gpio_set_direction(joystick_params->joystick->button, GPIO_MODE_INPUT);
    gpio_set_pull_mode(joystick_params->joystick->button, GPIO_PULLUP_ONLY);
    xTaskCreate(read_joystick_task, "read_joystick_task", 2048, joystick_params, 5, NULL);
}