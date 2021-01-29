/*
Copyright 2012-2020 Jun Wako, Jack Humbert, Yiancar, Ein Terakawa

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
#include <stdint.h>
#include <stdbool.h>
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"


#ifdef DIRECT_PINS
static const pin_t direct_pins[MATRIX_ROWS][MATRIX_COLS] = DIRECT_PINS;
#else

#    if (DIODE_DIRECTION == COL2ROW) || (DIODE_DIRECTION == ROW2COL)
#    elif (DIODE_DIRECTION == EITHERWAY) || (DIODE_DIRECTION == BOTHWAYS)
#    else
#        error DIODE_DIRECTION must be one of COL2ROW, ROW2COL, EITHERWAY or BOTHWAYS!
#    endif

static const pin_t row_pins[] = MATRIX_ROW_PINS;
static const pin_t col_pins[] = MATRIX_COL_PINS;
#define NUM_ROW_PINS (sizeof(row_pins)/sizeof(pin_t))
#define NUM_COL_PINS (sizeof(col_pins)/sizeof(pin_t))

/* Consistency checking of the size of the matrix and the number of pins */
// clang-format off
#    if (DIODE_DIRECTION == BOTHWAYS)
_Static_assert(NUM_ROW_PINS * 2 == MATRIX_ROWS, \
    "Number of elements in MATRIX_ROW_PINS * 2 must be equal to MATRIX_ROWS");
#    else
_Static_assert(NUM_ROW_PINS == MATRIX_ROWS, \
    "Number of elements in MATRIX_ROW_PINS must be equal to MATRIX_ROWS");
#    endif
_Static_assert(NUM_COL_PINS <= MATRIX_COLS, \
    "Number of elements in MATRIX_COL_PINS must be equal to or less than MATRIX_COLS");
// clang-format on
#endif

/* matrix state(1:on, 0:off) */
#if (DIODE_DIRECTION == EITHERWAY)
matrix_row_t last_matrix[MATRIX_ROWS]; // raw values of last scan
#endif
extern matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static inline void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON { setPinInputHigh(pin); }
}

// matrix code

#ifdef DIRECT_PINS

static void init_pins(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            pin_t pin = direct_pins[row][col];
            if (pin != NO_PIN) {
                setPinInputHigh(pin);
            }
        }
    }
}

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
        pin_t pin = direct_pins[current_row][col_index];
        if (pin != NO_PIN) {
            current_row_value |= readPin(pin) ? 0 : (MATRIX_ROW_SHIFTER << col_index);
        }
    }

    // If the row has changed, store the row and return the changed flag.
    if (current_matrix[current_row] != current_row_value) {
        current_matrix[current_row] = current_row_value;
        return true;
    }
    return false;
}

#else

static void unselect_row(uint8_t row) { setPinInputHigh_atomic(row_pins[row]); }

#    if (DIODE_DIRECTION != ROW2COL)

static void select_row(uint8_t row) { setPinOutput_writeLow(row_pins[row]); }

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    // Select row and wait for row selecton to stabilize
    select_row(current_row);
    matrix_io_delay();

    // For each col...
    for (uint8_t col_index = 0; col_index < NUM_COL_PINS; col_index++) {
        // Select the col pin to read (active low)
        uint8_t pin_state = readPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : (MATRIX_ROW_SHIFTER << col_index);
    }

    // Unselect row
    unselect_row(current_row);

    // If the row has changed, store the row and return the changed flag.
    if (current_matrix[current_row] != current_row_value) {
        current_matrix[current_row] = current_row_value;
        return true;
    }
    return false;
}

#    endif

static void unselect_col(uint8_t col) { setPinInputHigh_atomic(col_pins[col]); }

#    if (DIODE_DIRECTION != COL2ROW)

static void select_col(uint8_t col) { setPinOutput_writeLow(col_pins[col]); }

static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col) {
    bool matrix_changed = false;

    // Select col and wait for col selecton to stabilize
    select_col(current_col);
    matrix_io_delay();

    // For each row...
    for (uint8_t row_index = 0; row_index < NUM_ROW_PINS; row_index++) {
        // Store last value of row prior to reading
        matrix_row_t last_row_value    = current_matrix[row_index];
        matrix_row_t current_row_value = last_row_value;

        // Check row pin state
        if (readPin(row_pins[row_index]) == 0) {
            // Pin LO, set col bit
            current_row_value |= (MATRIX_ROW_SHIFTER << current_col);
        } else {
#        if (DIODE_DIRECTION != EITHERWAY)
            // Pin HI, clear col bit
            current_row_value &= ~(MATRIX_ROW_SHIFTER << current_col);
#        endif
        }

        // Determine if the matrix changed state
        if ((last_row_value != current_row_value)) {
            matrix_changed |= true;
            current_matrix[row_index] = current_row_value;
        }
    }

    // Unselect col
    unselect_col(current_col);

    return matrix_changed;
}

#    endif

static void unselect_rows(void) {
    for (uint8_t x = 0; x < NUM_ROW_PINS; x++) {
        unselect_row(x);
    }
}

static void unselect_cols(void) {
    for (uint8_t x = 0; x < NUM_COL_PINS; x++) {
        unselect_col(x);
    }
}

static void init_pins(void) {
    unselect_rows();
    unselect_cols();
}

#endif

void matrix_init(void) {
    // initialize key pins
    init_pins();

    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        raw_matrix[i] = 0;
        matrix[i]     = 0;
    }

    debounce_init(MATRIX_ROWS);

    matrix_init_quantum();
}

uint8_t matrix_scan(void) {
    bool changed = false;

#if defined(DIRECT_PINS) || (DIODE_DIRECTION == COL2ROW)
    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        changed |= read_cols_on_row(raw_matrix, current_row);
    }
#elif (DIODE_DIRECTION == ROW2COL)
    // Set col, read rows
    for (uint8_t current_col = 0; current_col < NUM_COL_PINS; current_col++) {
        changed |= read_rows_on_col(raw_matrix, current_col);
    }
#elif (DIODE_DIRECTION == EITHERWAY)
    // Logically combine the result of COL2ROW and ROW2COL
    for (uint8_t current_row = 0; current_row < NUM_ROW_PINS; current_row++) {
        last_matrix[current_row] = raw_matrix[current_row];
        read_cols_on_row(raw_matrix, current_row);
    }
    for (uint8_t current_col = 0; current_col < NUM_COL_PINS; current_col++) {
        read_rows_on_col(raw_matrix, current_col);
    }
    for (uint8_t current_row = 0; current_row < NUM_ROW_PINS; current_row++) {
        changed |= (last_matrix[current_row] != raw_matrix[current_row]);
    }
#elif (DIODE_DIRECTION == BOTHWAYS)
    // Concatinate the result of COL2ROW and ROW2COL, top to bottom
    for (uint8_t current_row = 0; current_row < NUM_ROW_PINS; current_row++) {
        changed |= read_cols_on_row(raw_matrix, current_row);
    }
    for (uint8_t current_col = 0; current_col < NUM_COL_PINS; current_col++) {
	changed |= read_rows_on_col(raw_matrix + NUM_ROW_PINS, current_col);
    }
#endif

    debounce(raw_matrix, matrix, MATRIX_ROWS, changed);

    matrix_scan_quantum();
    return (uint8_t)changed;
}
