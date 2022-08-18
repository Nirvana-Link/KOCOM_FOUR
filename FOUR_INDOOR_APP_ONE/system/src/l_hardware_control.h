#ifndef L_HARDWARE_CONTROL_H
#define L_HARDWARE_CONTROL_H

#include "gpio_api.h"

#define IO_SENSOR1_LEVEL_DETECT     (38)
#define IO_SENSOR2_LEVEL_DETECT     (39)

void gpio_detect_interupt_init(void);
void set_lcd_brightness(int brightness);
void start_door_unlock(void);
bool wait_door_unlock_finish(void);
GPIO_LEVEL sensor_gpio_level_get(const int pin);
#endif