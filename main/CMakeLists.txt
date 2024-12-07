idf_component_register(SRCS "main.c" "display/lcd1602.c" "display/joystick.c" "display/menu.c" "display/control_panel.c" "motion_system/motor.c" "motion_system/micro_switch.c" "motion_system/axis.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_gpio
                    REQUIRES esp_driver_i2c
                    REQUIRES driver)
