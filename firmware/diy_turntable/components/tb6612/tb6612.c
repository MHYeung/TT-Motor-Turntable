#include "tb6612.h"
#include "driver/ledc.h"
#include "esp_err.h"

static uint32_t duty_from_pct(uint8_t pct, uint32_t max_duty)
{
    if (pct > 100) pct = 100;
    return (max_duty * pct) / 100;
}

void tb6612_init(const tb6612_t *m)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL<<m->ain1) | (1ULL<<m->ain2) | (1ULL<<m->stby),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0, .pull_down_en = 0, .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io));

    gpio_set_level(m->stby, 1); // enable driver

    // LEDC PWM
    ledc_timer_config_t tcfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = m->ledc_timer,
        .duty_resolution = LEDC_TIMER_10_BIT, // 0..1023
        .freq_hz = m->pwm_hz,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&tcfg));

    ledc_channel_config_t ccfg = {
        .gpio_num = m->pwma,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = m->ledc_channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = m->ledc_timer,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ccfg));

    tb6612_set(m, 0, 0);
}

void tb6612_set(const tb6612_t *m, int dir, uint8_t duty_pct)
{
    // dir: -1 reverse, 0 brake/stop, +1 forward
    if (dir > 0) {
        gpio_set_level(m->ain1, 1);
        gpio_set_level(m->ain2, 0);
    } else if (dir < 0) {
        gpio_set_level(m->ain1, 0);
        gpio_set_level(m->ain2, 1);
    } else {
        // brake (both high) or coast (both low). Pick one:
        gpio_set_level(m->ain1, 0);
        gpio_set_level(m->ain2, 0); // coast
    }

    const uint32_t max_duty = (1u << 10) - 1; // 10-bit
    uint32_t duty = (dir == 0) ? 0 : duty_from_pct(duty_pct, max_duty);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, m->ledc_channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->ledc_channel);
}
