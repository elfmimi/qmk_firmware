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

/*
 * scan matrix
 */

#include "quantum.h"
#include "i2c_master.h"
#include "is31fl3741.h"


#if BUFFERED_DEBUG_PORT
void debugport_wait_buffer_empty(void); // in debugport.c
#endif

static const pin_t col_pins[MATRIX_COLS] = { D4, D3, C3, C2, C1, C0, B10, B9, C13, C12, C11, C10, C9, C8, D0, D1 };
static const pin_t row_pins[MATRIX_ROWS] = { B8, B14, A13, A14, B4, B5, B6, B7 };

static void init_rows_and_cols(void);
static matrix_row_t read_rows_on_col(void);
static inline void unselect_col(uint8_t col) { writePinLow(col_pins[col]); }
static inline void select_col(uint8_t col) { writePinHigh(col_pins[col]); }

void LED_Driver_DeactivateShutdown(void) {
    setPinOutput(A15); // PA.15 SDB
    writePinHigh(A15);
}

void LED_Driver_ActivateShutdown(void) {
    setPinOutput(A15); // PA.15 SDB
    writePinLow(A15);
}

// matrix_setup() is the earliest opportunity to do hardware initialization.
// before keyboard_pre_init_kb() / keyboard_init()
// before matrix_init() / matrix_init_custom()
void matrix_setup(void) {
    print("\n\nZ70 Pro firmware\n");

#if defined(IS31FL3741)
    LED_Driver_DeactivateShutdown();
#else
    LED_Driver_ActivateShutdown();
#endif
#if defined(IS31FL3741) && !defined(RGB_MATRIX_ENABLE)
    i2c_init();
    IS31FL3741_init(DRIVER_ADDR_1);
#endif
}

void matrix_init_custom(void) {
    // initialize row and col
    init_rows_and_cols();
}

#define STATUS_LED_SCALE 0x50
#define LED_SCALE 0x90

// At the end of matrix_init() / matrix_init_quantum()
void matrix_init_kb(void) {
#if defined(IS31FL3741)
    // We need to override RGB output scaling value
    for (int index = 0; index < DRIVER_LED_TOTAL; index++) {
	is31_led led = g_is31_leds[index];
	uint8_t scale = (index % 10) == 0 ? STATUS_LED_SCALE : LED_SCALE;
	IS31FL3741_set_scaling_registers(&led, scale, scale, scale);
    }
    IS31FL3741_update_led_control_registers(DRIVER_ADDR_1, 0);
#if !defined(RGB_MATRIX_ENABLE)
    // Clear buffer
    IS31FL3741_set_color_all(0, 0, 0);
    IS31FL3741_update_pwm_buffers(DRIVER_ADDR_1, DRIVER_ADDR_1);
#endif
#endif
    matrix_init_user();
}

#if 0
void keyboard_post_init_user(void);

// at the end of keyboard_init()
void keyboard_post_init_kb(void) {
    // debug
    // debug_matrix = true;
    keyboard_post_init_user();
}
#endif

bool matrix_scan_custom(matrix_row_t raw_matrix[]) {
    bool changed = false;

    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        select_col(i);
	wait_us(1);
        matrix_row_t rows = read_rows_on_col();
        unselect_col(i);
	
	for (uint8_t j = 0; j < MATRIX_ROWS; j++) {
	    uint8_t ii = i, jj = j;
	    if (((raw_matrix[jj] >> ii) & 1) != ((rows >> j) & 1)) {
		changed = true;
		raw_matrix[jj] ^= (1 << ii);
	    }
	}
    }

    return changed;
}

#if 0
void matrix_print(void) {
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
	// libc of newlib library does not support "%b"
	// but we are using implementation of tfp_printf()
        pbin_reverse16(matrix_get_row(row));
        // printf("\n"); // This printf() will be optimized to putchar() then eventually call _write() .
	print("\n");
    }
}
#endif

// Matrix column-pins and row-pins configuration
static void  init_rows_and_cols(void) {
    for (int c=0; c < MATRIX_COLS; c++) {
	setPinOutput(col_pins[c]);
        writePinLow(col_pins[c]);
    }

    for (int r = 0; r < MATRIX_ROWS; r++) {
	setPinInput(row_pins[r]);
    }

    // disable input on PB.7 == R7
    PB->OFFD  |= (1 << (7 + 16));
}

static matrix_row_t read_rows_on_col(void) {
    matrix_row_t row = 0;
    for (int r = 0; r < MATRIX_ROWS; r++) {
	row |= (readPin(row_pins[r]) << r);
    }
    return row;
}
