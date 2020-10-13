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

#include "quantum.h"
#include "is31fl3741.h"

const is31_led g_is31_leds[DRIVER_LED_TOTAL] = {
    {0, CS3_SW1,  CS1_SW1,  CS2_SW1  }, // CapsLock Indicator
    {0, CS4_SW1,  CS6_SW1,  CS5_SW1  },
    {0, CS7_SW1,  CS9_SW1,  CS8_SW1  },
    {0, CS10_SW1, CS12_SW1, CS11_SW1 },
    {0, CS13_SW1, CS15_SW1, CS14_SW1 },
    {0, CS16_SW1, CS18_SW1, CS17_SW1 },
    {0, CS19_SW1, CS21_SW1, CS20_SW1 },
    {0, CS22_SW1, CS24_SW1, CS23_SW1 },
    {0, CS25_SW1, CS27_SW1, CS26_SW1 },
    {0, CS28_SW1, CS30_SW1, CS29_SW1 },
    {0, CS3_SW2,  CS1_SW2,  CS2_SW2  }, // NumLock Indicator
    {0, CS4_SW2,  CS6_SW2,  CS5_SW2  },
    {0, CS7_SW2,  CS9_SW2,  CS8_SW2  },
    {0, CS10_SW2, CS12_SW2, CS11_SW2 },
    {0, CS13_SW2, CS15_SW2, CS14_SW2 },
    {0, CS16_SW2, CS18_SW2, CS17_SW2 },
    {0, CS19_SW2, CS21_SW2, CS20_SW2 },
    {0, CS22_SW2, CS24_SW2, CS23_SW2 },
    {0, CS25_SW2, CS27_SW2, CS26_SW2 },
    {0, CS28_SW2, CS30_SW2, CS29_SW2 },
    {0, CS3_SW3,  CS1_SW3,  CS2_SW3  }, // ScrollLock Indicator
    {0, CS4_SW3,  CS6_SW3,  CS5_SW3  },
    {0, CS7_SW3,  CS9_SW3,  CS8_SW3  },
    {0, CS10_SW3, CS12_SW3, CS11_SW3 },
    {0, CS13_SW3, CS15_SW3, CS14_SW3 },
    {0, CS16_SW3, CS18_SW3, CS17_SW3 },
    {0, CS19_SW3, CS21_SW3, CS20_SW3 },
    {0, CS22_SW3, CS24_SW3, CS23_SW3 },
    {0, CS25_SW3, CS27_SW3, CS26_SW3 },
    {0, CS28_SW3, CS30_SW3, CS29_SW3 },
    {0, CS1_SW4,  CS3_SW4,  CS2_SW4  },
    {0, CS4_SW4,  CS6_SW4,  CS5_SW4  },
    {0, CS7_SW4,  CS9_SW4,  CS8_SW4  },
    {0, CS10_SW4, CS12_SW4, CS11_SW4 },
    {0, CS13_SW4, CS15_SW4, CS14_SW4 },
    {0, CS16_SW4, CS18_SW4, CS17_SW4 },
    {0, CS19_SW4, CS21_SW4, CS20_SW4 },
    {0, CS22_SW4, CS24_SW4, CS23_SW4 },
    {0, CS25_SW4, CS27_SW4, CS26_SW4 },
    {0, CS28_SW4, CS30_SW4, CS29_SW4 },
    {0, CS1_SW5,  CS3_SW5,  CS2_SW5  },
    {0, CS4_SW5,  CS6_SW5,  CS5_SW5  },
    {0, CS7_SW5,  CS9_SW5,  CS8_SW5  },
    {0, CS10_SW5, CS12_SW5, CS11_SW5 },
    {0, CS13_SW5, CS15_SW5, CS14_SW5 },
    {0, CS16_SW5, CS18_SW5, CS17_SW5 },
    {0, CS19_SW5, CS21_SW5, CS20_SW5 },
    {0, CS22_SW5, CS24_SW5, CS23_SW5 },
    {0, CS25_SW5, CS27_SW5, CS26_SW5 },
    {0, CS28_SW5, CS30_SW5, CS29_SW5 },
    {0, CS1_SW6,  CS3_SW6,  CS2_SW6  },
    {0, CS4_SW6,  CS6_SW6,  CS5_SW6  },
    {0, CS7_SW6,  CS9_SW6,  CS8_SW6  },
    {0, CS10_SW6, CS12_SW6, CS11_SW6 },
    {0, CS13_SW6, CS15_SW6, CS14_SW6 },
    {0, CS16_SW6, CS18_SW6, CS17_SW6 },
    {0, CS19_SW6, CS21_SW6, CS20_SW6 },
    {0, CS22_SW6, CS24_SW6, CS23_SW6 },
    {0, CS25_SW6, CS27_SW6, CS26_SW6 },
    {0, CS28_SW6, CS30_SW6, CS29_SW6 },
    {0, CS1_SW7,  CS3_SW7,  CS2_SW7  },
    {0, CS4_SW7,  CS6_SW7,  CS5_SW7  },
    {0, CS7_SW7,  CS9_SW7,  CS8_SW7  },
    {0, CS10_SW7, CS12_SW7, CS11_SW7 },
    {0, CS13_SW7, CS15_SW7, CS14_SW7 },
    {0, CS16_SW7, CS18_SW7, CS17_SW7 },
    {0, CS19_SW7, CS21_SW7, CS20_SW7 },
    {0, CS22_SW7, CS24_SW7, CS23_SW7 },
    {0, CS25_SW7, CS27_SW7, CS26_SW7 },
    {0, CS28_SW7, CS30_SW7, CS29_SW7 },
    {0, CS1_SW8,  CS3_SW8,  CS2_SW8  },
    {0, CS4_SW8,  CS6_SW8,  CS5_SW8  },
    {0, CS7_SW8,  CS9_SW8,  CS8_SW8  },
    {0, CS10_SW8, CS12_SW8, CS11_SW8 },
    {0, CS13_SW8, CS15_SW8, CS14_SW8 },
    {0, CS16_SW8, CS18_SW8, CS17_SW8 },
    {0, CS19_SW8, CS21_SW8, CS20_SW8 },
    {0, CS22_SW8, CS24_SW8, CS23_SW8 },
    {0, CS25_SW8, CS27_SW8, CS26_SW8 },
    {0, CS28_SW8, CS30_SW8, CS29_SW8 },
    {0, CS1_SW9,  CS3_SW9,  CS2_SW9  },
    {0, CS4_SW9,  CS6_SW9,  CS5_SW9  },
    {0, CS7_SW9,  CS9_SW9,  CS8_SW9  },
    {0, CS10_SW9, CS12_SW9, CS11_SW9 },
    {0, CS13_SW9, CS15_SW9, CS14_SW9 },
    {0, CS16_SW9, CS18_SW9, CS17_SW9 },
    {0, CS19_SW9, CS21_SW9, CS20_SW9 },
    {0, CS22_SW9, CS24_SW9, CS23_SW9 },
    {0, CS25_SW9, CS27_SW9, CS26_SW9 },
    {0, CS28_SW9, CS30_SW9, CS29_SW9 },
};

// byte order: R,G,B
static uint8_t caps_lock_color[3] = { 0x00, 0xFF, 0x00 };
static uint8_t num_lock_color[3] = { 0xFF, 0x00, 0x00 };
static uint8_t scroll_lock_color[3] = { 0x00, 0x00, 0xFF };

static uint8_t led_status;

void led_set(uint8_t usb_led)
{
    led_status = usb_led;

    {
	// Z70 Pro
	// We know our status leds are on PAGE0
#ifdef RGB_MATRIX_ENABLE
	// STATUS0 (CAPS)
	if (led_status >> USB_LED_CAPS_LOCK & 1) {
	    rgb_matrix_set_color(0, caps_lock_color[0], caps_lock_color[1], caps_lock_color[2]);
	} else {
	    rgb_matrix_set_color(0, 0, 0, 0);
	}
	// STATUS1 (NumLock)
	if (led_status >> USB_LED_NUM_LOCK & 1) {
	    rgb_matrix_set_color(10, num_lock_color[0], num_lock_color[1], num_lock_color[2]);
	} else {
	    rgb_matrix_set_color(10, 0, 0, 0);
	}
	// STATUS2 (ScrollLock)
	if (led_status >> USB_LED_SCROLL_LOCK & 1) {
	    rgb_matrix_set_color(20, scroll_lock_color[0], scroll_lock_color[1], scroll_lock_color[2]);
	} else {
	    rgb_matrix_set_color(20, 0, 0, 0);
	}
#else
	// STATUS0 (CAPS)
	if (led_status >> USB_LED_CAPS_LOCK & 1) {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].r, caps_lock_color[0]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].g, caps_lock_color[1]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].b, caps_lock_color[2]);
	} else {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].r, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].g, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[0].b, 0);
	}
	// STATUS1 (NumLock)
	if (led_status >> USB_LED_NUM_LOCK & 1) {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].r, num_lock_color[0]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].g, num_lock_color[1]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].b, num_lock_color[2]);
	} else {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].r, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].g, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[10].b, 0);
	}
	// STATUS2 (ScrollLock)
	if (led_status >> USB_LED_SCROLL_LOCK & 1) {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].r, scroll_lock_color[0]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].g, scroll_lock_color[1]);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].b, scroll_lock_color[2]);
	} else {
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFE, 0xC5);
	    IS31FL3741_write_register(DRIVER_ADDR_1, 0xFD, 0x00);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].r, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].g, 0);
	    IS31FL3741_write_register(DRIVER_ADDR_1, g_is31_leds[20].b, 0);
	}
#endif	
    }
}
