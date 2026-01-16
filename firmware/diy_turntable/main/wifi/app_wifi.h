#pragma once
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

esp_err_t app_wifi_init_sta(void);
esp_err_t app_wifi_wait_connected(TickType_t timeout);
