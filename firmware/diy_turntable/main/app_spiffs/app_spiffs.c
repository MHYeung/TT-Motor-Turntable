#include "app_spiffs.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "app_spiffs";

esp_err_t app_spiffs_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 8,
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) return ret;

    size_t total=0, used=0;
    esp_spiffs_info("spiffs", &total, &used);
    ESP_LOGI(TAG, "SPIFFS mounted: total=%u used=%u", (unsigned)total, (unsigned)used);
    return ESP_OK;
}
