#include "app_web.h"
#include "app_motor.h"

#include "esp_http_server.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *TAG = "app_web";

static esp_err_t send_file(httpd_req_t *req, const char *path, const char *ctype)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, ctype);

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (httpd_resp_send_chunk(req, buf, n) != ESP_OK) break;
    }
    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t root_get(httpd_req_t *req) { return send_file(req, "/spiffs/index.html", "text/html"); }
static esp_err_t css_get(httpd_req_t *req)  { return send_file(req, "/spiffs/style.css", "text/css"); }
static esp_err_t js_get(httpd_req_t *req)   { return send_file(req, "/spiffs/app.js", "application/javascript"); }

static inline int clamp_int(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static esp_err_t api_motor_get(httpd_req_t *req)
{
    // Always allow browser calls (harmless when same-origin; useful if you later test from elsewhere)
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");

    motor_state_t cur = app_motor_get();
    int dir = cur.dir;
    int speed = cur.speed_pct;

    char qs[128] = {0};
    if (httpd_req_get_url_query_str(req, qs, sizeof(qs)) == ESP_OK) {
        char v[16];

        if (httpd_query_key_value(qs, "dir", v, sizeof(v)) == ESP_OK) {
            dir = atoi(v);
        }
        if (httpd_query_key_value(qs, "speed", v, sizeof(v)) == ESP_OK) {
            speed = atoi(v);
        }

        // Validate / clamp
        dir = clamp_int(dir, -1, 1);
        speed = clamp_int(speed, 0, 100);

        // If stopped, force speed 0 (safer behavior)
        if (dir == 0) speed = 0;

        app_motor_set(dir, (uint8_t)speed);
    }

    motor_state_t st = app_motor_get();

    char resp[96];
    snprintf(resp, sizeof(resp),
             "{\"ok\":true,\"speed\":%u,\"dir\":%d}",
             st.speed_pct, st.dir);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


esp_err_t app_web_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8192;
    config.lru_purge_enable = true;
    config.max_open_sockets = 6;


    httpd_handle_t server = NULL;
    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) return err;

    httpd_uri_t u_root = { .uri="/", .method=HTTP_GET, .handler=root_get };
    httpd_uri_t u_css  = { .uri="/style.css", .method=HTTP_GET, .handler=css_get };
    httpd_uri_t u_js   = { .uri="/app.js", .method=HTTP_GET, .handler=js_get };
    httpd_uri_t u_api  = { .uri="/api/motor", .method=HTTP_GET, .handler=api_motor_get };

    httpd_register_uri_handler(server, &u_root);
    httpd_register_uri_handler(server, &u_css);
    httpd_register_uri_handler(server, &u_js);
    httpd_register_uri_handler(server, &u_api);

    ESP_LOGI(TAG, "Web server started.");
    return ESP_OK;
}
