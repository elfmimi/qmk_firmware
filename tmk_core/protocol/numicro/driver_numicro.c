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

#include "quantum.h"
#include "numicro_protocol.h"
#include "usbd_hid.h"

static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

host_driver_t numicro_driver = {keyboard_leds, send_keyboard, send_mouse, send_system, send_consumer};

static uint8_t keyboard_leds(void) {
    return USBD_HID_keyboard_leds();
}

static void send_keyboard(report_keyboard_t *report) {
    if (!USBD_HID_isConfigured()) {
        return;
    }

    const int ep = EP2;
    do {
	if (USBD_HID_isSuspended()) {
	    return;
	}
    } while(g_u8EPisBusy[ep]);
    {
        g_u8EPisBusy[ep] = 1;
        uint8_t *buf = (uint8_t *)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep);
        uint8_t *report_raw = report->raw;
        uint32_t len = KEYBOARD_REPORT_SIZE;
        USBD_MemCopy(buf, report_raw, len);
        USBD_SET_PAYLOAD_LEN(ep, len);
    }
}

#ifdef MOUSE_ENABLE
static void send_mouse(report_mouse_t *report) {
    if (!USBD_HID_isConfigured()) {
        return;
    }
#if !defined(MOUSE_SHARED_EP)
    const int ep = EP3;
#elif !defined(KEYBOARD_SHARED_EP)
    const int ep = EP4;
#else
    const int ep = EP2;
#endif
    do {
	if (USBD_HID_isSuspended()) {
	    return;
	}
    } while(g_u8EPisBusy[ep]);
    {
        g_u8EPisBusy[ep] = 1;
        uint8_t *buf = (uint8_t *)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep);
        uint8_t *report_raw = (uint8_t *)report;
        uint32_t len = sizeof(report_mouse_t);
        USBD_MemCopy(buf, report_raw, len);
        USBD_SET_PAYLOAD_LEN(ep, len);
    }
}
#else
static void send_mouse(report_mouse_t *report) {
}
#endif

#ifdef EXTRAKEY_ENABLE
static void send_extra_report(uint8_t report_id, uint16_t data) {
    if (!USBD_HID_isConfigured()) {
        return;
    }
    report_extra_t report = {.report_id = report_id, .usage = data};

#if !defined(KEYBOARD_SHARED_EP)
    const int ep = EP4;
#else
    const int ep = EP2;
#endif
    do {
	if (USBD_HID_isSuspended()) {
	    return;
	}
    } while(g_u8EPisBusy[ep]);
    {
        g_u8EPisBusy[ep] = 1;
        uint8_t *buf = (uint8_t *)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep);
        uint8_t *report_raw = (uint8_t *)&report;
        uint32_t len = sizeof(report_extra_t);
        USBD_MemCopy(buf, report_raw, len);
        USBD_SET_PAYLOAD_LEN(ep, len);
    }
}

static void send_system(uint16_t data) { send_extra_report(REPORT_ID_SYSTEM, data); }

static void send_consumer(uint16_t data) { send_extra_report(REPORT_ID_CONSUMER, data); }

#else  /* EXTRAKEY_ENABLE */
static void send_system(uint16_t data) { (void)data; }
static void send_consumer(uint16_t data) { (void)data; }
#endif /* EXTRAKEY_ENABLE */
