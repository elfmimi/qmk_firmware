/*
    NuMicro Protocol - Copyright (C) 2019 Ein Terakawa

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "numicro_protocol.h"

#if defined(DEBUG_ENABLE_SEMIHOST) || defined(OS_USE_SEMIHOSTING)
#error You may want to use implementation from retarget.c to utilize SEMIHOSTING.
#endif

// By the effect of system_NUCxxx.h , DEBUG_PORT default to UART0.
// NO_DEBUG_PORT will override that.

#if defined(NO_DEBUG_PORT)

void sendchar_debugport(uint8_t) __attribute__((alias("sendchar_debugport_discard")));
void sendchar_debugport_discard(uint8_t ch) { }

#elif !defined(BUFFERED_DEBUG_PORT)

void sendchar_debugport(uint8_t ch) {
    do {} while(UART_IS_TX_FULL(DEBUG_PORT));
    UART_WRITE(DEBUG_PORT, ch);
    if (ch == '\n') {
        sendchar_debugport('\r');
    }
}

#else

/* Buffered and less-blocking implementation of sendchar for UART output */
#ifndef DEBUGPORT_BUF_LEN
#define DEBUGPORT_BUF_LEN 1024
#endif
static uint8_t debugport_buf[DEBUGPORT_BUF_LEN];
static int debugport_write_idx = 0;
static int debugport_read_idx = 0;

void debugport_fill_fifo(void) {
    while (debugport_read_idx != debugport_write_idx && !UART_IS_TX_FULL(DEBUG_PORT)) {
        UART_WRITE(DEBUG_PORT, debugport_buf[debugport_read_idx++]);
        debugport_read_idx %= DEBUGPORT_BUF_LEN;
    };
}

void sendchar_debugport(uint8_t) __attribute__((alias("sendchar_debugport_buffered")));
void sendchar_debugport_buffered(uint8_t ch) {
    if ((debugport_write_idx + 1) % DEBUGPORT_BUF_LEN == debugport_read_idx) {
        // buffer full -> would block
        do {} while(UART_IS_TX_FULL(DEBUG_PORT));
        UART_WRITE(DEBUG_PORT, debugport_buf[debugport_read_idx++]);
        debugport_read_idx %= DEBUGPORT_BUF_LEN;
    }
    debugport_buf[debugport_write_idx++] = ch;
    debugport_write_idx %= DEBUGPORT_BUF_LEN;
        
    if (ch == '\n') {
        sendchar_debugport_buffered('\r');
        return;
    }

    debugport_fill_fifo();
}

// Wait till all data get sent.
void debugport_wait_buffer_empty(void) {
    do {
        debugport_fill_fifo();
    } while(!UART_IS_TX_EMPTY(DEBUG_PORT));
}
#endif /* BUFFERED_DEBUG_PORT */
