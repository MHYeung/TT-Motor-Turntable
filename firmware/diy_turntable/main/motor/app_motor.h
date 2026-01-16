#pragma once
#include "esp_err.h"
#include <stdint.h>

typedef struct {
    int dir;           // -1, 0, +1
    uint8_t speed_pct; // 0..100
} motor_state_t;

esp_err_t app_motor_init(void);
esp_err_t app_motor_set(int dir, uint8_t speed_pct);
motor_state_t app_motor_get(void);
