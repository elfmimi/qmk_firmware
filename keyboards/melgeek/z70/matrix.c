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


#if BUFFERED_DEBUG_PORT
void debugport_wait_buffer_empty(void); // in debugport.c
#endif

static const pin_t col_pins[MATRIX_COLS] = { D4, D3, C3, C2, C1, C0, B10, B9, C13, C12, C11, C10, C9, C8, D0, D1 };
static const pin_t row_pins_z70_pro[MATRIX_ROWS] = { B8, B14, A13, A14, B4, B5, B6, B7 };
static const pin_t row_pins_z70[MATRIX_ROWS] = { B8, B14, A11, A10, B4, B5, B6, B7 };
static const pin_t *row_pins;

static bool is_z70_pro = false;
bool isZ70Pro(void) {
    return is_z70_pro;
}

static void init_rows_and_cols(void);
static matrix_row_t read_rows_on_col(void);
static inline void unselect_col(uint8_t col) { writePinLow(col_pins[col]); }
static inline void select_col(uint8_t col) { writePinHigh(col_pins[col]); }

void LED_Driver_ActivateShutdown(void) {
    setPinOutput(A15); // PA.15 SDB
    writePinLow(A15);
}

// matrix_setup() is the earliest opportunity to do hardware initialization.
// before keyboard_pre_init_kb() / keyboard_init()
// before matrix_init() / matrix_init_custom()
void matrix_setup(void) {
    print("\n\nZ70 firmware\n");

    setPinInput(A10);
    wait_ms(5);
    if (0 != readPin(A10)) {
	print("This hardware is Z70 Pro.\n");
	is_z70_pro = true;
	LED_Driver_ActivateShutdown();
    } else {
	print("This hardware is Z70.\n");
	is_z70_pro = false;
	setPinOutput(A14); // PA.14 Caps LED
	writePinHigh(A14);
	setPinOutput(A15); // PA.15 rest of the LEDs
	writePinHigh(A15);
    }
    row_pins = is_z70_pro ? row_pins_z70_pro : row_pins_z70;
}

void matrix_init_custom(void) {
    // initialize row and col
    init_rows_and_cols();
}

#if 0
// At the end of matrix_init() / matrix_init_quantum()
void matrix_init_kb(void) {
    matrix_init_user();
}
#endif

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
	    if (!is_z70_pro) {
		// remap Z70 layout to Z70 Pro layout
		switch (j * MATRIX_COLS + i) {
		case (4 * MATRIX_COLS + 8):
		    jj = 4; ii = 7; break;
		case (4 * MATRIX_COLS + 10):
		    jj = 4; ii = 9; break;
		case (4 * MATRIX_COLS + 12):
		    jj = 4; ii = 11; break;
		case (5 * MATRIX_COLS + 11):
		    jj = 6; ii = 11; break;
		case (5 * MATRIX_COLS + 13):
		    jj = 5; ii = 11; break;
		case (6 * MATRIX_COLS + 11):
		    jj = 5; ii = 13; break;
		}
	    }
	    if (((raw_matrix[jj] >> ii) & 1) != ((rows >> j) & 1)) {
		changed = true;
		raw_matrix[jj] ^= (1 << ii);
	    }
	}
    }

    return changed;
}

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
