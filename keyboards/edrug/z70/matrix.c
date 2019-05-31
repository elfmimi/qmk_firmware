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

#include "ch.h"
#include "hal.h"
#include "hal_queues.h"

/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"
#include "debounce.h"
#include "printf.h"
#include "sendchar.h"
#include "z70_led.h"

static bool is_z70_pro = false;
bool isZ70Pro(void) {
    return is_z70_pro;
}

/* matrix state(1:on, 0:off) */
static matrix_row_t raw_matrix[MATRIX_ROWS]; //raw values
static matrix_row_t matrix[MATRIX_ROWS]; //debounced values

static matrix_row_t read_rows_on_col(void);
static void init_rows_and_cols(void);
static void unselect_col(uint8_t row);
static void select_col(uint8_t row);

inline
uint8_t matrix_rows(void) {
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void) {
    return MATRIX_COLS;
}

void sendchar_serial(void *p, char c) {
    (void)p;
    sdWrite(&SD1, (unsigned char *)&c, 1U);
#ifdef CONSOLE_ENABLE
    sendchar((uint8_t)c);
#endif
}

void wait_serial_output(void) {
    while (1) {
	osalSysLock();
	bool b = oqIsEmptyI(&(&SD1)->oqueue);
	osalSysUnlock();
	if (b) break;
	wait_ms(1);
    } 
}

static void init_i2c(void) {
    CLK->APBCLK |= CLK_APBCLK_I2C0_EN_Msk;
    SYS->IPRSTC2 |= SYS_IPRSTC2_I2C0_RST_Msk;
    I2C0->I2CON = I2C_I2CON_ENS1_Msk;
    // printf("I2CSTATUS = %08X\n", I2C0->I2CSTATUS);
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_I2C0_RST_Msk;
    // printf("I2CSTATUS = %08X\n", I2C0->I2CSTATUS);
    i2cInit();
    printf("after i2cInit()\n");
    wait_serial_output();
    I2CConfig i2c_config = { 100000 };
    i2cStart(&I2CD1, &i2c_config);
    printf("after i2cStart()\n");
    wait_serial_output();
}

void LED_Driver_DeactivateShutdown(void) {
    palSetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL); // PA.15 SDB
    palSetPad(GPIOA, 15);
}

void LED_Driver_ActivateShutdown(void) {
    palSetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL); // PA.15 SDB
    palClearPad(GPIOA, 15);
}

// before matrix_init() 
void matrix_setup(void) {
    // Set PC.4 , PC.5 as UART0_RXD , UART0_TXD pin
    SYS->GPC_MFP |= 0x30;
    SYS->ALT_MFP |= 0x60000000;
    // Set PF.2 , PF.3 as I2C0_SDA , I2C0_SCL
    SYS->GPF_MFP |= 0x0C;
    SYS->ALT_MFP1 = (SYS->ALT_MFP1 & 0xF0FFFFFF) | 0x0A000000;

    sdStart(&SD1, NULL);
    init_printf(NULL,sendchar_serial);
    printf("\n\nZ70 Pro / Z70 firmware\n");
    wait_serial_output();

    palSetPadMode(GPIOA, 10, PAL_MODE_INPUT);
    wait_us(1);
    if (PAL_LOW != palReadPad(GPIOA, 10)) {
	printf("This hardware is Z70 Pro.\n");
	is_z70_pro = true;
	init_i2c();
	LED_Driver_DeactivateShutdown();
	LED_Driver_Init();
    } else {
	printf("This hardware is Z70.\n");
	is_z70_pro = false;
	palSetPadMode(GPIOA, 14, PAL_MODE_OUTPUT_PUSHPULL); // PA.14 Caps LED
	palSetPad(GPIOA, 14);
	palSetPadMode(GPIOA, 15, PAL_MODE_OUTPUT_PUSHPULL); // PA.15 rest of the LEDs
	palSetPad(GPIOA, 15);
    }
    wait_serial_output();

    init_printf(NULL,NULL);
}

__attribute__ ((weak))
void matrix_init_user(void) {
}

__attribute__ ((weak))
void matrix_scan_user(void) {
}

void matrix_init_kb(void) {
    matrix_init_user();
}

void matrix_scan_kb(void) {
    matrix_scan_user();
}

void matrix_init(void) {
    // initialize row and col
    init_rows_and_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
	raw_matrix[i] = 0;
        matrix[i] = 0;
    }

    debounce_init(MATRIX_ROWS);

    matrix_init_quantum();
}

void keyboard_post_init_user(void);

void keyboard_post_init_kb(void) {
    //debug
    //debug_matrix = true;
    // sdStart(&SD1, NULL);
    // Note: Doing this at this point we'll miss "USB configured.\n" message.
    // But there is no other place better than here.
    init_printf(NULL,sendchar_serial);
    keyboard_post_init_user();
}

static int count = 0;

uint8_t matrix_scan(void) {
    if (count < 1000) {
	count ++;
	for (uint8_t i=0; i < MATRIX_ROWS; i++) {
	    raw_matrix[i] = 0;
	    matrix[i] = 0;
	}
	return 0;
    }

    bool changed = false;

    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        select_col(i);
        select_col(i);
        select_col(i);
        // wait_us(30);  // without this wait read unstable value.
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

    debounce(raw_matrix, matrix, MATRIX_ROWS, changed);

    matrix_scan_quantum();
    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row) {
    return matrix[row];
}

void matrix_print(void) {
    printf("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        printf("%02X: ", row);
	matrix_row_t row_bits = matrix_get_row(row);
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {
	    printf((row_bits & (1 << col)) ? "1" : "0");
	}
        printf("\n");
    }
}

/* Column pin configuration
 */
static void  init_rows_and_cols(void) {
    palSetPadMode(GPIOD, 1, PAL_MODE_OUTPUT_PUSHPULL); // C15
    palSetPadMode(GPIOD, 0, PAL_MODE_OUTPUT_PUSHPULL); // C14
    palSetPadMode(GPIOC, 8, PAL_MODE_OUTPUT_PUSHPULL); // C13
    palSetPadMode(GPIOC, 9, PAL_MODE_OUTPUT_PUSHPULL); // C12
    palSetPadMode(GPIOC, 10, PAL_MODE_OUTPUT_PUSHPULL); // C11
    palSetPadMode(GPIOC, 11, PAL_MODE_OUTPUT_PUSHPULL); // C10
    palSetPadMode(GPIOC, 12, PAL_MODE_OUTPUT_PUSHPULL); // C9
    palSetPadMode(GPIOC, 13, PAL_MODE_OUTPUT_PUSHPULL); // C8
    palSetPadMode(GPIOB, 9, PAL_MODE_OUTPUT_PUSHPULL); // C7
    palSetPadMode(GPIOB, 10, PAL_MODE_OUTPUT_PUSHPULL); // C6
    palSetPadMode(GPIOC, 0, PAL_MODE_OUTPUT_PUSHPULL); // C5
    palSetPadMode(GPIOC, 1, PAL_MODE_OUTPUT_PUSHPULL); // C4
    palSetPadMode(GPIOC, 2, PAL_MODE_OUTPUT_PUSHPULL); // C3
    palSetPadMode(GPIOC, 3, PAL_MODE_OUTPUT_PUSHPULL); // C2
    palSetPadMode(GPIOD, 3, PAL_MODE_OUTPUT_PUSHPULL); // C1
    palSetPadMode(GPIOD, 4, PAL_MODE_OUTPUT_PUSHPULL); // C0
    palClearPad(GPIOD, 1);
    palClearPad(GPIOD, 0);
    palClearPad(GPIOC, 8);
    palClearPad(GPIOC, 9);
    palClearPad(GPIOC, 10);
    palClearPad(GPIOC, 11);
    palClearPad(GPIOC, 12);
    palClearPad(GPIOC, 13);
    palClearPad(GPIOB, 9);
    palClearPad(GPIOB, 10);
    palClearPad(GPIOC, 0);
    palClearPad(GPIOC, 1);
    palClearPad(GPIOC, 2);
    palClearPad(GPIOC, 3);
    palClearPad(GPIOD, 3);
    palClearPad(GPIOD, 4);

    // don't need pullup/down, since it's pulled down in hardware
    palSetPadMode(GPIOB, 8, PAL_MODE_INPUT); // R0
    palSetPadMode(GPIOB, 14, PAL_MODE_INPUT); // R1
    palSetPadMode(GPIOA, is_z70_pro ? 13 : 11, PAL_MODE_INPUT); // R2
    palSetPadMode(GPIOA, is_z70_pro ? 14 : 10, PAL_MODE_INPUT); // R3
    palSetPadMode(GPIOB, 4, PAL_MODE_INPUT); // R4
    palSetPadMode(GPIOB, 5, PAL_MODE_INPUT); // R5
    palSetPadMode(GPIOB, 6, PAL_MODE_INPUT); // R6
    palSetPadMode(GPIOB, 7, PAL_MODE_INPUT); // R7

    // disable input on PB.7 == R7
    GPIOB->OFFD  |= (1 << (7 + 16));
}

static matrix_row_t read_rows_on_col(void) {
    return
	((palReadPad(GPIOB, 8)==PAL_LOW) ? 0 : (1<<0)) |
	((palReadPad(GPIOB, 14)==PAL_LOW) ? 0 : (1<<1)) |
	((palReadPad(GPIOA, is_z70_pro ? 13 : 11)==PAL_LOW) ? 0 : (1<<2)) |
	((palReadPad(GPIOA, is_z70_pro ? 14 : 10)==PAL_LOW) ? 0 : (1<<3)) |
	((palReadPad(GPIOB, 4)==PAL_LOW) ? 0 : (1<<4)) |
	((palReadPad(GPIOB, 5)==PAL_LOW) ? 0 : (1<<5)) |
	((palReadPad(GPIOB, 6)==PAL_LOW) ? 0 : (1<<6)) |
	((palReadPad(GPIOB, 7)==PAL_LOW) ? 0 : (1<<7));
}


/* Col pin configuration
 */
static void unselect_col(uint8_t row) {
    switch(row) {
    case 15:
	palClearPad(GPIOD, 1); 
	break;
    case 14:
	palClearPad(GPIOD, 0); 
	break;
    case 13:
	palClearPad(GPIOC, 8);
	break;
    case 12:
	palClearPad(GPIOC, 9);
	break;
    case 11:
	palClearPad(GPIOC, 10);
	break;
    case 10:
	palClearPad(GPIOC, 11);
	break;
    case 9:
	palClearPad(GPIOC, 12);
	break;
    case 8:
	palClearPad(GPIOC, 13);
	break;
    case 7:
	palClearPad(GPIOB, 9);
	break;
    case 6:
	palClearPad(GPIOB, 10);
	break;
    case 5:
	palClearPad(GPIOC, 0);
	break;
    case 4:
	palClearPad(GPIOC, 1);
	break;
    case 3:
	palClearPad(GPIOC, 2);
	break;
    case 2:
	palClearPad(GPIOC, 3);
	break;
    case 1:
	palClearPad(GPIOD, 3);
	break;
    case 0:
	palClearPad(GPIOD, 4);
	break;
    }
}

static void select_col(uint8_t row) {
    switch(row) {
    case 15:
	palSetPad(GPIOD, 1); 
	break;
    case 14:
	palSetPad(GPIOD, 0); 
	break;
    case 13:
	palSetPad(GPIOC, 8);
	break;
    case 12:
	palSetPad(GPIOC, 9);
	break;
    case 11:
	palSetPad(GPIOC, 10);
	break;
    case 10:
	palSetPad(GPIOC, 11);
	break;
    case 9:
	palSetPad(GPIOC, 12);
	break;
    case 8:
	palSetPad(GPIOC, 13);
	break;
    case 7:
	palSetPad(GPIOB, 9);
	break;
    case 6:
	palSetPad(GPIOB, 10);
	break;
    case 5:
	palSetPad(GPIOC, 0);
	break;
    case 4:
	palSetPad(GPIOC, 1);
	break;
    case 3:
	palSetPad(GPIOC, 2);
	break;
    case 2:
	palSetPad(GPIOC, 3);
	break;
    case 1:
	palSetPad(GPIOD, 3);
	break;
    case 0:
	palSetPad(GPIOD, 4);
	break;
    }
}
