#pragma once
#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t ain1;
    gpio_num_t ain2;
    gpio_num_t stby;
    gpio_num_t pwma;     // PWM output pin
    int ledc_timer;      // LEDC timer index
    int ledc_channel;    // LEDC channel index
    uint32_t pwm_hz;     // e.g. 20000
} tb6612_t;

void tb6612_init(const tb6612_t *m);
void tb6612_set(const tb6612_t *m, int dir, uint8_t duty_pct); // dir: -1,0,+1  duty:0-100
