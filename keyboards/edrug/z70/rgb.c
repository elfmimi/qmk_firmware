/*
Copyright 2019 Ein Terakawa <applause@elfmimi.jp>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "rgb.h"
#include "rgb_matrix.h"
#include "rgb_matrix_types.h"
#include "z70_led.h"

#define XX NO_LED

led_config_t g_led_config = {
    {
        {  9, 19, 29, 39, 49, 59, 69, 79, 89,  8, 18, 28, 38, 48, 88, XX },
        {  7, 17, 27, 37, 47, 57, 67, 77, 87,  6, 16, 26, 36, 46, XX, XX },
        {  5, 15, 25, 35, 45, 55, 65, 75, 85, 86, 76, 66, 56, XX, XX, XX },
        { 12, XX, 62, 32, 52, 63, 73, 83, 23,  3, 53, 54, 64, XX, XX, XX },
        {  2, 22, 42, XX, XX, 13, XX, 33, XX, 43,  4, 14, XX, XX, XX, XX },
        { XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, 58, 68, 74, 78, XX },
        { XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, 84, 24, 34, 44, XX },
        { XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX },
    }, {
        {  0,  0 }, {  0,  0 }, {  2, 64 }, {148, 48 }, {174, 64 }, {  6, 32 }, {152, 16 }, {  4, 16 }, {144,  0 }, {  0,  0 }, 
        {  0,  0 }, {  0,  0 }, { 10, 48 }, { 75, 64 }, {190, 64 }, { 28, 32 }, {168, 16 }, { 24, 16 }, {160,  0 }, { 16,  0 }, 
        {  0,  0 }, {  0,  0 }, { 22, 64 }, {132, 48 }, {208, 64 }, { 44, 32 }, {184, 16 }, { 40, 16 }, {176,  0 }, { 32,  0 }, 
        {  0,  0 }, {  0,  0 }, { 52, 48 }, {123, 64 }, {224, 64 }, { 60, 32 }, {200, 16 }, { 56, 16 }, {192,  0 }, { 48,  0 }, 
        {  0,  0 }, {  0,  0 }, { 42, 64 }, {156, 64 }, {240, 64 }, { 76, 32 }, {220, 16 }, { 72, 16 }, {208,  0 }, { 64,  0 }, 
        {  0,  0 }, {  0,  0 }, { 68, 48 }, {164, 48 }, {180, 48 }, { 92, 32 }, {214, 32 }, { 88, 16 }, {240,  0 }, { 80,  0 }, 
        {  0,  0 }, {  0,  0 }, { 36, 48 }, { 84, 48 }, {202, 48 }, {108, 32 }, {188, 32 }, {104, 16 }, {240, 16 }, { 96,  0 }, 
        {  0,  0 }, {  0,  0 }, {  0,  0 }, {100, 48 }, {224, 48 }, {124, 32 }, {172, 32 }, {120, 16 }, {240, 32 }, {112,  0 }, 
        {  0,  0 }, {  0,  0 }, {  0,  0 }, {116, 48 }, {240, 48 }, {140, 32 }, {156, 32 }, {136, 16 }, {224,  0 }, {128,  0 }, 
    }, {
        4, 4, 1, 4, 1, 4, 4, 4, 4, 4,
        4, 4, 1, 4, 1, 4, 4, 4, 4, 4,
        4, 4, 1, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 1, 1, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 1, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    }
};

LED_TYPE led[DRIVER_LED_TOTAL];

static void init(void) {}

void led_test(void);

static void flush(void) {
    SET_ALL_but_status_PWM(led);
}

static void set_color(int i, uint8_t r, uint8_t g, uint8_t b) {
    led[i].r = r;
    led[i].g = g;
    led[i].b = b;
}

static void set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < sizeof(led) / sizeof(led[0]); i++) {
        led[i].r = r;
        led[i].g = g;
        led[i].b = b;
    }
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = init,
    .set_color     = set_color,
    .set_color_all = set_color_all,
    .flush         = flush,
};
