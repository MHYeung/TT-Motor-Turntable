idf_component_register(
    SRCS 
        "main.c"
        "wifi/app_wifi.c"
        "motor/app_motor.c"
        "web/app_web.c"
        "app_spiffs/app_spiffs.c"
                    
    INCLUDE_DIRS 
        "."
        "motor/"
        "app_spiffs/"
        "web/"
        "wifi/"
                    
    REQUIRES 
        driver
        tb6612
        esp_http_server
        nvs_flash
        esp_wifi
        esp_netif
        spiffs
    )
