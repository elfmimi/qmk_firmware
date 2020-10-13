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

static uint8_t led_status;

bool isZ70Pro(void);
void led_set(uint8_t usb_led)
{
    led_status = usb_led;

    if (isZ70Pro()) {
	// Z70 Pro
    } else {
	// Z70 has only CAPS indicator
	if (led_status >> USB_LED_CAPS_LOCK & 1) {
	    writePinLow(A14);
	} else {
	    writePinHigh(A14);
	}
    }
}
