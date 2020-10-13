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

#ifndef __USBD_HID_H__
#define __USBD_HID_H__

#include <stdbool.h>

// #define EP0_MAX_PKT_SIZE    8
#define EP0_MAX_PKT_SIZE    FIXED_CONTROL_ENDPOINT_SIZE
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
#if !defined(KEYBOARD_SHARED_EP)
#define EP2_MAX_PKT_SIZE    KEYBOARD_EPSIZE
#else
#define EP2_MAX_PKT_SIZE    SHARED_EPSIZE
#endif
#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
#define EP3_MAX_PKT_SIZE    MOUSE_EPSIZE
#else
#define EP3_MAX_PKT_SIZE    0
#endif
#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
#define EP4_MAX_PKT_SIZE    SHARED_EPSIZE
#else
#define EP4_MAX_PKT_SIZE    0
#endif
#define EP5_MAX_PKT_SIZE    CONSOLE_EPSIZE

#define SETUP_BUF_BASE  0
#define SETUP_BUF_LEN   8
#define EP0_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN     EP0_MAX_PKT_SIZE
#define EP1_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN     EP1_MAX_PKT_SIZE
#define EP2_BUF_BASE    (EP1_BUF_BASE + EP1_BUF_LEN)
#define EP2_BUF_LEN     EP2_MAX_PKT_SIZE
#define EP3_BUF_BASE    (EP2_BUF_BASE + EP2_BUF_LEN)
#define EP3_BUF_LEN     EP3_MAX_PKT_SIZE
#define EP4_BUF_BASE    (EP3_BUF_BASE + EP3_BUF_LEN)
#define EP4_BUF_LEN     EP4_MAX_PKT_SIZE
#define EP5_BUF_BASE    (EP4_BUF_BASE + EP4_BUF_LEN)
#define EP5_BUF_LEN     EP5_MAX_PKT_SIZE

extern uint8_t volatile g_u8EPisBusy[8];

uint8_t USBD_HID_keyboard_leds(void);
void USBD_HID_Start(void);
bool USBD_HID_isConfigured(void);
void USBD_HID_Poll(void);
bool USBD_HID_isSuspended(void);
void USBD_HID_RemoteWakeup(void);

#endif  /* __USBD_HID_H__ */
