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

#ifdef CONSOLE_ENABLE

#include "quantum.h"
#include "numicro_protocol.h"
#include "usbd_hid.h"
#include "usb_descriptor.h"

#ifndef CONSOLE_BUF_LEN
#define CONSOLE_BUF_LEN 1024
#endif
static uint8_t hid_console_buf[CONSOLE_BUF_LEN];
static int hid_con_write_idx = 0;
static int hid_con_read_idx = 0;
static int delay_count = 1000;

void sendchar_console(uint8_t c) {
    if ((hid_con_write_idx + 1) % CONSOLE_BUF_LEN == hid_con_read_idx) {
        // buffur full -> throw away the input
        return;
    }
    hid_console_buf[hid_con_write_idx++] = c;
    hid_con_write_idx %= CONSOLE_BUF_LEN;
}

uint8_t read_console_buf(void) {
    if (hid_con_read_idx == hid_con_write_idx) {
        return 0;
    }
    uint8_t c = hid_console_buf[hid_con_read_idx++];
    hid_con_read_idx %= CONSOLE_BUF_LEN;
    return c;
}

void console_task(void) {
    if (!USBD_HID_isConfigured()) {
        return;
    }
    const int ep = EP5;
    if(!g_u8EPisBusy[ep]){
        // print("send_console\n");
        g_u8EPisBusy[ep] = 1;
        uint8_t *buf = (uint8_t *)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep);
        int i = 0;
        if (delay_count != 0) {
            delay_count -= 1;
        } else {
            for ( ; i < CONSOLE_EPSIZE; i++) {
                uint8_t c = read_console_buf();
                buf[i] = c;
                if (c == 0)
                    break;
            }
        }
        for ( ; i < CONSOLE_EPSIZE; i++) {
            buf[i] = 0;
        }
        USBD_SET_PAYLOAD_LEN(ep, CONSOLE_EPSIZE);
    }
}

#endif /* CONSOLE_ENABLE */
