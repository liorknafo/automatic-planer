#include "driver/gpio.h"

void init_micro_switch(uint8_t pin) {
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

uint8_t micro_switch_pressed(uint8_t pin) {
    return gpio_get_level(pin) == 1;
}