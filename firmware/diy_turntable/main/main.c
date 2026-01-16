#include "esp_log.h"
#include "nvs_flash.h"

#include "app_wifi.h"
#include "app_spiffs.h"
#include "app_web.h"
#include "app_motor.h"

static const char *TAG = "app_main";

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    // Motor first (safe default = stopped)
    ESP_ERROR_CHECK(app_motor_init());

    // Wi-Fi
    ESP_ERROR_CHECK(app_wifi_init_sta());
    ESP_ERROR_CHECK(app_wifi_wait_connected(pdMS_TO_TICKS(15000))); // optional wait

    // SPIFFS (web assets)
    ESP_ERROR_CHECK(app_spiffs_init());

    // Web server
    ESP_ERROR_CHECK(app_web_start());

    ESP_LOGI(TAG, "System ready.");
}
