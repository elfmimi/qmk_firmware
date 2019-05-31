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

#include "hal.h"
#include "led.h"

static uint8_t led_status;

bool isZ70Pro(void);

#define LED_DRIVER_ADDR 0x60

void write_reg(uint8_t adr, uint8_t dat) {
    uint8_t txbuf[2];
    txbuf[0] = adr;
    txbuf[1] = dat;
    msg_t msg = i2cMasterTransmitTimeout(&I2CD1, LED_DRIVER_ADDR >> 1, txbuf, 2, NULL, 0, MS2ST(5));
    (void)msg;
}

uint8_t read_reg(uint8_t adr) {
    uint8_t txrxbuf[1];
    txrxbuf[0] = adr;
    msg_t msg = i2cMasterTransmitTimeout(&I2CD1, LED_DRIVER_ADDR >> 1, txrxbuf, 1, txrxbuf, 1, MS2ST(5));
    (void)msg;
    return txrxbuf[0];
}

void SET_ALL_PWM(uint8_t pwm) {
    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x00);
    for (int i = 0x00; i<=0xB3; i++) {
	write_reg(i, pwm);
    }
    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x01);
    for (int i = 0x00; i<=0xAA; i++) {
	write_reg(i, pwm);
    }
}

void SET_ALL_Scale(uint8_t sl) {
    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x02);
    static const uint8_t STATUS_LED_LIMIT = 0x40;
    for (int i = 0x00; i<=0xB3; i++) {
	if (((i >= 0 && i <= 2) || (i >= 0x1E && i <= 0x20) || (i >= 0x3C && i <= 0x3E))) {
	    if (sl > STATUS_LED_LIMIT) {
		write_reg(i, STATUS_LED_LIMIT);
	    } else {
		write_reg(i, sl);
	    }
	} else {
	    write_reg(i, sl);
	}
    }
    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x03);
    for (int i = 0x00; i<=0xAA; i++) {
	write_reg(i, sl);
    }
}

void LED_Driver_Init(void) {
    SET_ALL_PWM(0x00);
    SET_ALL_Scale(0xFF);
#if 0
    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x00);
    // byte order: G,B,R
    // STATUS0 (CAPS)
    for (int i = 0x00; i<=0x02; i++) {
	write_reg(i, 0xFF);
    }
    // STATUS1 (NumLock)
    for (int i = 0x1E; i<=0x20; i++) {
	write_reg(i, 0xFF);
    }
    // STATUS2 (ScrollLock)
    for (int i = 0x3C; i<=0x3E; i++) {
	write_reg(i, 0xFF);
    }
#endif

    write_reg(0xFE, 0xC5);
    write_reg(0xFD, 0x04);
    write_reg(0x01, 0x90); // Global Current Control
    write_reg(0x00, 0x01); // SSD <- 1 , Normal Operation
}

// byte order: G,B,R
static uint8_t caps_lock_color[3] = { 0x00, 0x00, 0xFF };
static uint8_t num_lock_color[3] = { 0xFF, 0x00, 0x00 };
static uint8_t scroll_lock_color[3] = { 0x00, 0xFF, 0x00 };

void led_set(uint8_t usb_led)
{
    led_status = usb_led;

    if (isZ70Pro()) {
	// Z70 Pro
	write_reg(0xFE, 0xC5);
	write_reg(0xFD, 0x00);
	// STATUS0 (CAPS)
	for (int i = 0; i < 3; i++) {
	    write_reg(0x00 + i, (led_status >> USB_LED_CAPS_LOCK & 1) ? caps_lock_color[i] : 0x00);
	}
	// STATUS1 (NumLock)
	for (int i = 0; i < 3; i++) {
	    write_reg(0x1E + i, (led_status >> USB_LED_NUM_LOCK & 1) ? num_lock_color[i] : 0x00);
	}
	// STATUS2 (ScrollLock)
	for (int i = 0; i < 3; i++) {
	    write_reg(0x3C + i, (led_status >> USB_LED_SCROLL_LOCK & 1) ? scroll_lock_color[i] : 0x00);
	}
    } else {
	// Z70 has only CAPS indicator
	if (led_status >> USB_LED_CAPS_LOCK & 1) {
	    palClearPad(GPIOA, 14);
	} else {
	    palSetPad(GPIOA, 14);
	}
    }
}
