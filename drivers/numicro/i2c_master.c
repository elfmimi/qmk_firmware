/* Copyright 2018 Jack Humbert
 * Copyright 2018 Yiancar
 * Copyright 2020 Ein Terakawa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This library follows the convention of the AVR i2c_master library.
 * As a result addresses are expected to be already shifted (addr << 1).
 */

#include "i2c_master.h"
#include "quantum.h"
#include <string.h>

static uint8_t i2c_address;

__attribute__((weak)) I2CConfig i2cconfig = { 100000 };

static i2c_status_t chibios_to_qmk(const msg_t* status) {
    switch (*status) {
        case I2C_NO_ERROR:
            return I2C_STATUS_SUCCESS;
        case I2C_TIMEOUT:
            return I2C_STATUS_TIMEOUT;
        // I2C_BUS_ERROR, I2C_ARBITRATION_LOST, I2C_ACK_FAILURE, I2C_OVERRUN, I2C_PEC_ERROR, I2C_SMB_ALERT
        default:
            return I2C_STATUS_ERROR;
    }
}

__attribute__((weak)) void i2c_init(void) {
    i2cInit(); // No prorer hal means we don't have halInit(). We do need to invoke this here.
}

i2c_status_t i2c_start(uint8_t address) {
    i2c_address = address;
    i2cStart(&I2C_DRIVER, &i2cconfig);
    return I2C_STATUS_SUCCESS;
}

i2c_status_t i2c_transmit(uint8_t address, const uint8_t* data, uint16_t length, uint16_t timeout) {
    i2c_address = address;
    i2cStart(&I2C_DRIVER, &i2cconfig);
    msg_t status = i2cMasterTransmitTimeout(&I2C_DRIVER, (i2c_address >> 1), data, length, 0, 0, MS2ST(timeout));
    return chibios_to_qmk(&status);
}

i2c_status_t i2c_receive(uint8_t address, uint8_t* data, uint16_t length, uint16_t timeout) {
    i2c_address = address;
    i2cStart(&I2C_DRIVER, &i2cconfig);
    msg_t status = i2cMasterReceiveTimeout(&I2C_DRIVER, (i2c_address >> 1), data, length, MS2ST(timeout));
    return chibios_to_qmk(&status);
}

i2c_status_t i2c_writeReg(uint8_t devaddr, uint8_t regaddr, const uint8_t* data, uint16_t length, uint16_t timeout) {
    i2c_address = devaddr;
    i2cStart(&I2C_DRIVER, &i2cconfig);

    uint8_t complete_packet[length + 1];
    for (uint8_t i = 0; i < length; i++) {
        complete_packet[i + 1] = data[i];
    }
    complete_packet[0] = regaddr;

    msg_t status = i2cMasterTransmitTimeout(&I2C_DRIVER, (i2c_address >> 1), complete_packet, length + 1, 0, 0, MS2ST(timeout));
    return chibios_to_qmk(&status);
}

i2c_status_t i2c_readReg(uint8_t devaddr, uint8_t regaddr, uint8_t* data, uint16_t length, uint16_t timeout) {
    i2c_address = devaddr;
    i2cStart(&I2C_DRIVER, &i2cconfig);
    msg_t status = i2cMasterTransmitTimeout(&I2C_DRIVER, (i2c_address >> 1), &regaddr, 1, data, length, MS2ST(timeout));
    return chibios_to_qmk(&status);
}

void i2c_stop(void) { i2cStop(&I2C_DRIVER); }
