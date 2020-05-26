#include "z70ultra.h"
#include "config.h"
#include "is31fl3741.h"

#include <print.h>

const is31_led g_is31_leds[DRIVER_LED_TOTAL] = {
    {0, 28, 1, 30, 1, 29, 1}, /* RGB10 */
    {0, 28, 2, 30, 2, 29, 2}, /* RGB11 */
    {0, 28, 3, 30, 3, 29, 3}, /* RGB12 */
    {0, 28, 4, 30, 4, 29, 4}, /* RGB13 */
    {0, 28, 5, 30, 5, 29, 5}, /* RGB14 */
    {0, 28, 6, 30, 6, 29, 6}, /* RGB15 */
    {0, 28, 7, 30, 7, 29, 7}, /* RGB16 */
    {0, 28, 8, 30, 8, 29, 8}, /* RGB17 */
    {0, 28, 9, 30, 9, 29, 9}, /* RGB18 */
    {0, 31, 1, 33, 1, 32, 1}, /* RGB28 */
    {0, 31, 2, 33, 2, 32, 2}, /* RGB29 */
    {0, 31, 3, 33, 3, 32, 3}, /* RGB30 */
    {0, 31, 4, 33, 4, 32, 4}, /* RGB31 */
    {0, 31, 5, 33, 5, 32, 5}, /* RGB32 */
    {0, 31, 6, 33, 6, 32, 6}, /* RGB33 */
    {0, 25, 1, 27, 1, 26, 1}, /* RGB1 */
    {0, 25, 2, 27, 2, 26, 2}, /* RGB2 */
    {0, 25, 3, 27, 3, 26, 3}, /* RGB3 */
    {0, 25, 4, 27, 4, 26, 4}, /* RGB4 */
    {0, 25, 5, 27, 5, 26, 5}, /* RGB5 */
    {0, 25, 6, 27, 6, 26, 6}, /* RGB6 */
    {0, 25, 7, 27, 7, 26, 7}, /* RGB7 */
    {0, 25, 8, 27, 8, 26, 8}, /* RGB8 */
    {0, 25, 9, 27, 9, 26, 9}, /* RGB9 */
    {0, 34, 1, 36, 1, 35, 1}, /* RGB46 */
    {0, 34, 2, 36, 2, 35, 2}, /* RGB47 */
    {0, 34, 3, 36, 3, 35, 3}, /* RGB48 */
    {0, 34, 4, 36, 4, 35, 4}, /* RGB49 */
    {0, 34, 5, 36, 5, 35, 5}, /* RGB50 */
    {0, 31, 7, 33, 7, 32, 7}, /* RGB34 */
    {0, 22, 1, 24, 1, 23, 1}, /* RGB37 */
    {0, 22, 2, 24, 2, 23, 2}, /* RGB38 */
    {0, 22, 3, 24, 3, 23, 3}, /* RGB39 */
    {0, 22, 4, 24, 4, 23, 4}, /* RGB40 */
    {0, 22, 5, 24, 5, 23, 5}, /* RGB41 */
    {0, 22, 6, 24, 6, 23, 6}, /* RGB42 */
    {0, 22, 7, 24, 7, 23, 7}, /* RGB43 */
    {0, 22, 8, 24, 8, 23, 8}, /* RGB44 */
    {0, 22, 9, 24, 9, 23, 9}, /* RGB45 */
    {0, 34, 9, 36, 9, 35, 9}, /* RGB54 */
    {0, 34, 8, 36, 8, 35, 8}, /* RGB53 */
    {0, 34, 7, 36, 7, 35, 7}, /* RGB52 */
    {0, 34, 6, 36, 6, 35, 6}, /* RGB51 */
    {0, 31, 8, 33, 8, 32, 8}, /* RGB35 */
    {0, 19, 2, 21, 2, 20, 2}, /* RGB56 */
    {0, 19, 4, 21, 4, 20, 4}, /* RGB58 */
    {0, 19, 5, 21, 5, 20, 5}, /* RGB59 */
    {0, 19, 6, 21, 6, 20, 6}, /* RGB60 */
    {0, 19, 7, 21, 7, 20, 7}, /* RGB61 */
    {0, 19, 8, 21, 8, 20, 8}, /* RGB62 */
    {0, 19, 9, 21, 9, 20, 9}, /* RGB63 */
    {0, 19, 3, 21, 3, 20, 3}, /* RGB57 */
    {0, 19, 1, 21, 1, 20, 1}, /* RGB55 */
    {0, 37, 5, 39, 5, 38, 5}, /* RGB23 */
    {0, 37, 6, 39, 6, 38, 6}, /* RGB24 */
    {0, 37, 7, 39, 7, 38, 7}, /* RGB25 */
    {0, 37, 8, 39, 8, 38, 8}, /* RGB26 */
    {0, 31, 9, 33, 9, 32, 9}, /* RGB36 */
    {0, 6, 1, 8, 1, 7, 1},    /* RGB64 */
    {0, 6, 3, 8, 3, 7, 3},    /* RGB66 */
    {0, 6, 4, 8, 4, 7, 4},    /* RGB67 */
    {0, 6, 5, 8, 5, 7, 5},    /* RGB69 */
    {0, 6, 6, 8, 6, 7, 6},    /* RGB70 */
    {0, 6, 2, 8, 2, 7, 2},    /* RGB65 */
    {0, 37, 1, 39, 1, 38, 1}, /* RGB19 */
    {0, 37, 2, 39, 2, 38, 2}, /* RGB20 */
    {0, 37, 3, 39, 3, 38, 3}, /* RGB21 */
    {0, 37, 4, 39, 4, 38, 4}, /* RGB22 */
    {0, 37, 9, 39, 9, 38, 9}, /* RGB27 */
};

led_config_t g_led_config = {{
                                 /*C0      C1      C2          C3      C4      C5      C6      C7  C8      C9       C10      C11        C12    C13  */
                                 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},                                   /* R0 */
                                 {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28},                         /* R1 */
                                 {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, NO_LED, 42},                     /* R2 */
                                 {44, NO_LED, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, NO_LED},                 /* R3 */
                                 {58, 59, 60, NO_LED, 61, 62, 63, NO_LED, NO_LED, 64, 65, NO_LED, NO_LED, NO_LED}, /* R4 */
                                 {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, 14, 29, 56, 43, 57, 66, 67, 68}  /* R5 */
                             },
                             {
                                 {0, 0}, {16, 0}, {32, 0}, {48, 0}, {64, 0}, {80, 0}, {96, 0}, {112, 0}, {128, 0}, {144, 0}, {160, 0}, {176, 0}, {192, 0}, {208, 0}, {224, 0}, {0, 16}, {16, 16}, {32, 16}, {48, 16}, {64, 16}, {80, 16}, {96, 16}, {112, 16}, {128, 16}, {144, 16}, {160, 16}, {176, 16}, {192, 16}, {208, 16}, {224, 16}, {0, 32}, {16, 32}, {32, 32}, {48, 32}, {64, 32}, {80, 32}, {96, 32}, {112, 32}, {128, 32}, {144, 32}, {160, 32}, {176, 32}, {192, 32}, {208, 32}, {0, 48}, {16, 48}, {32, 48}, {48, 48}, {64, 48}, {80, 48}, {96, 48}, {112, 48}, {128, 48}, {144, 48}, {160, 48}, {176, 48}, {192, 48}, {208, 48}, {0, 64}, {16, 64}, {32, 64}, {48, 64}, {64, 64}, {80, 64}, {96, 64}, {112, 64}, {128, 64}, {144, 64}, {160, 64},
                             },
                             {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};

const is31_led g_is31_indicator_leds[6] = {
    {0, 5, 1, 3, 1, 4, 1}, /* RGB71 */
    {0, 5, 2, 3, 2, 4, 2}, /* RGB72 */
    {0, 5, 3, 3, 3, 4, 3}, /* RGB73 */
    {0, 5, 4, 3, 4, 4, 4}, /* RGB74 */
    {0, 5, 5, 3, 5, 4, 5}, /* RGB75 */
    {0, 5, 6, 3, 6, 4, 6}, /* RGB76 */
};

void led_set_user(uint8_t usb_led) {
    // uprintf("------------------------led_set_user %x\r\n", usb_led);
    if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[2], 0xff, 0x00, 0x00);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[3], 0xff, 0x00, 0x00);
    } else {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[2], 0x00, 0x00, 0x00);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[3], 0x00, 0x00, 0x00);
    }

    if (usb_led & (1 << USB_LED_NUM_LOCK)) {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[1], 0x00, 0xff, 0x00);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[4], 0x00, 0xff, 0x00);
    } else {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[1], 0x00, 0x00, 0x00);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[4], 0x00, 0x00, 0x00);
    }

    if (usb_led & (1 << USB_LED_SCROLL_LOCK)) {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[0], 0x00, 0x00, 0xff);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[5], 0x00, 0x00, 0xff);
    } else {
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[0], 0x00, 0x00, 0x00);
        IS31FL3741_set_pwm_buffer(&g_is31_indicator_leds[5], 0x00, 0x00, 0x00);
    }

    // flush the indicator to the buffer
    IS31FL3741_update_pwm_buffers(DRIVER_ADDR_1, DRIVER_ADDR_1);
}

void matrix_init_kb(void) {
    for (int i = 0; i < DRIVER_INDICATOR_LED_TOTAL; ++i) {
        is31_led led = g_is31_indicator_leds[i];

        IS31FL3741_set_scaling_registers(&led, 0xFF, 0xFF, 0xFF);
    }

    IS31FL3741_update_led_control_registers(DRIVER_ADDR_1, 0);

    matrix_init_user();
}

void matrix_scan_kb(void) { matrix_scan_user(); }

bool process_record_kb(uint16_t keycode, keyrecord_t *record) { return process_record_user(keycode, record); }

void suspend_power_down_kb(void) {
    rgb_matrix_set_suspend_state(true);
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
    rgb_matrix_set_suspend_state(false);
    suspend_wakeup_init_user();
}

void matrix_setup(void) {}
