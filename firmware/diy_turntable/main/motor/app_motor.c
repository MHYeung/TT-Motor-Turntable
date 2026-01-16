#include "app_motor.h"
#include "tb6612.h"

#include "esp_log.h"
#include "driver/ledc.h"

static const char *TAG = "app_motor";

static motor_state_t s_state = { .dir = 0, .speed_pct = 0 };
static tb6612_t s_motor;

static ledc_timer_t pick_timer(void)
{
#if CONFIG_TB6612_LEDC_TIMER_0
    return LEDC_TIMER_0;
#elif CONFIG_TB6612_LEDC_TIMER_1
    return LEDC_TIMER_1;
#elif CONFIG_TB6612_LEDC_TIMER_2
    return LEDC_TIMER_2;
#else
    return LEDC_TIMER_0;
#endif
}

static ledc_channel_t pick_channel(void)
{
#if CONFIG_TB6612_LEDC_CHANNEL_0
    return LEDC_CHANNEL_0;
#elif CONFIG_TB6612_LEDC_CHANNEL_1
    return LEDC_CHANNEL_1;
#elif CONFIG_TB6612_LEDC_CHANNEL_2
    return LEDC_CHANNEL_2;
#elif CONFIG_TB6612_LEDC_CHANNEL_3
    return LEDC_CHANNEL_3;
#else
    return LEDC_CHANNEL_0;
#endif
}

esp_err_t app_motor_init(void)
{
    s_motor.ain1 = (gpio_num_t)CONFIG_TB6612_AIN1_GPIO;
    s_motor.ain2 = (gpio_num_t)CONFIG_TB6612_AIN2_GPIO;
    s_motor.stby = (gpio_num_t)CONFIG_TB6612_STBY_GPIO;
    s_motor.pwma = (gpio_num_t)CONFIG_TB6612_PWMA_GPIO;

    s_motor.ledc_timer   = (int)pick_timer();
    s_motor.ledc_channel = (int)pick_channel();
    s_motor.pwm_hz       = (uint32_t)CONFIG_TB6612_PWM_FREQ_HZ;

    tb6612_init(&s_motor);
    tb6612_set(&s_motor, 0, 0);

    ESP_LOGI(TAG, "TB6612 init: AIN1=%d AIN2=%d STBY=%d PWMA=%d PWM=%uHz",
             s_motor.ain1, s_motor.ain2, s_motor.stby, s_motor.pwma, (unsigned)s_motor.pwm_hz);

    return ESP_OK;
}

esp_err_t app_motor_set(int dir, uint8_t speed_pct)
{
    if (dir < -1) dir = -1;
    if (dir > 1)  dir = 1;
    if (speed_pct > 100) speed_pct = 100;

    // if stop, force speed 0
    if (dir == 0) speed_pct = 0;

    s_state.dir = dir;
    s_state.speed_pct = speed_pct;

    tb6612_set(&s_motor, dir, speed_pct);
    return ESP_OK;
}

motor_state_t app_motor_get(void)
{
    return s_state;
}
