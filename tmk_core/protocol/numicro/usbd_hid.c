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
#include "usbd_hid.h"
#include "usb_descriptor.h"
#include "printf.h"  // lib/printf/printf.h

uint8_t volatile g_u8EPisBusy[USBD_MAX_EP] = { 0, 0, 0, 0, 0, 0, 0, 0 };

#ifdef COMMAND_ENABLE
// dummy to satisfy tmk_core/common/command.c
uint8_t keyboard_idle = 0;
uint8_t keyboard_protocol = 1;
#endif

static uint8_t configured = 0;
static uint8_t is_set_report = 0;
static uint8_t keyboard_led_stats = 0;
static uint8_t suspended = 0;

static S_USBD_INFO_T usbdInfo;
static const uint8_t *gpu8UsbString[4] = { 0, 0, 0, 0 };
static const uint8_t *gpu8UsbHidReport[4] = { 0, 0, 0, 0 };
static uint32_t gu32UsbHidReportLen[4] = { 0, 0, 0, 0 };
static uint32_t gu32ConfigHidDescIdx[4] = { 0, 0, 0, 0 };

#ifdef SUPPORT_LPM
static const uint8_t gu8UsbBosDecs[12] = { // This is not tested.
    4, // bLength
    15, // bDescriptorType DTYPE_BOS
    12, 0, // wTotalLength 5 + 7
    1, // bNumDeviceCaps

    7, // bLength
    16, // bDescriptorType DTYPE_DeviceCapability
    2, // bDevCapabilityType USB 2.0 Extension
    0, 0, 0, 0, // bmAttributes LPM not supported
};
#endif

/* LED status */
uint8_t USBD_HID_keyboard_leds(void) { return keyboard_led_stats; }

static void Process_HID_SetReport_Payload(int ep);

void USBD_IRQHandler(void) {
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

    if (u32IntSts & USBD_INTSTS_FLDET) {
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

        if (USBD_IS_ATTACHED()) {
            USBD_ENABLE_USB();
        } else {
            USBD_DISABLE_USB();
        }
    }

    if (u32IntSts & USBD_INTSTS_WAKEUP) {
        USBD_CLR_INT_FLAG(USBD_INTSTS_WAKEUP);
    }

    if (u32IntSts & USBD_INTSTS_BUS) {
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

        if (u32State & USBD_STATE_USBRST) {
            USBD_ENABLE_USB();
            USBD_SwReset();
        }
        if (u32State & USBD_STATE_SUSPEND) {
	    if (configured) {
		USBD_DISABLE_PHY();
		suspended = 1;
		printf("USB Suspended.\n");
	    } else {
		printf("Not Configured.\n");
	    }
        }
        if (u32State & USBD_STATE_RESUME) {
	    printf("USB Resumed.\n");
	    suspended = 0;
            USBD_ENABLE_USB(); // This will also enable PHY.
        }
    }

    // Handle USB Event
    if (u32IntSts & USBD_INTSTS_USB) {
        // Handle Setup Packet
        if (u32IntSts & USBD_INTSTS_SETUP) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

            is_set_report = 0;

            USBD_ProcessSetupPacket();
        }

        // Handle EP0 : Control IN
        if (u32IntSts & USBD_INTSTS_EP0) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
            USBD_CtrlIn();
        }

        // Handle EP1 : Control OUT
        if (u32IntSts & USBD_INTSTS_EP1) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            if (is_set_report) {
                Process_HID_SetReport_Payload(EP1);
                is_set_report = 0;
            } else {
                USBD_CtrlOut();
            }
        }

        // Handle EP2 : Interrupt IN
        if (u32IntSts & USBD_INTSTS_EP2) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);
            g_u8EPisBusy[EP2] = 0;
        }

        // Handle EP3 : Interrupt IN
        if (u32IntSts & USBD_INTSTS_EP3) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);
            g_u8EPisBusy[EP3] = 0;
        }

        // Handle EP4 : Interrupt IN
        if (u32IntSts & USBD_INTSTS_EP4) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
            g_u8EPisBusy[EP4] = 0;
        }

        // Handle EP5 : Interrupt IN
        if (u32IntSts & USBD_INTSTS_EP5) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
            g_u8EPisBusy[EP5] = 0;
        }

        // EP6 : Unused
        if (u32IntSts & USBD_INTSTS_EP6) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP6);
            g_u8EPisBusy[EP6] = 0;
        }

        // EP7 : Unused
        if (u32IntSts & USBD_INTSTS_EP7) {
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP7);
            g_u8EPisBusy[EP7] = 0;
        }
    }
}

/* Configure USBD endpoints */
static void Configure_Endpoints(void) {
    // SETUP packet buffer
    USBD->STBUFSEG = SETUP_BUF_BASE;

    // EP0 : Control IN ( 0x80 )
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    // EP1 : Control OUT ( 0x00 )
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    // EP2 : Interrupt IN ( 0x81 )
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | KEYBOARD_IN_EPNUM);
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    // EP3 : Interrupt IN ( 0x82 )
    USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_IN | MOUSE_IN_EPNUM);
    USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
#endif

#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
    // EP4 : Interrupt IN ( 0x83 or 0x85 )
    USBD_CONFIG_EP(EP4, USBD_CFG_EPMODE_IN | SHARED_IN_EPNUM);
    USBD_SET_EP_BUF_ADDR(EP4, EP4_BUF_BASE);
#endif

#if defined(CONSOLE_ENABLE)
    // EP5 : Interrupt IN ( 0x82 , 0x83 , 0x84 or 0x86 )
    USBD_CONFIG_EP(EP5, USBD_CFG_EPMODE_IN | CONSOLE_IN_EPNUM);
    USBD_SET_EP_BUF_ADDR(EP5, EP5_BUF_BASE);
#endif
}

/* Callback to process SETUP packets of Class Requests */
static void Process_ClassRequest(void) {
    uint8_t buf[8];

    USBD_GetSetupPacket(buf);

    uint8_t bmRequestType = buf[0];
    uint8_t bRequest = buf[1];
    uint8_t bReportType = buf[3]; // wValue [15:8]
    // Here, it is always bmRequestType.Class .
    bool is_get = (bmRequestType & 0x80) != 0; // bmRequestType.DeviceToHost
    bool is_interface = (bmRequestType & 0x1F) == 1; // bmRequestType.Interface

    if (!is_interface) {
        // Unexpected -> STALL
        USBD_SET_EP_STALL(EP0);
        USBD_SET_EP_STALL(EP1);
        return;
    }

    if (is_get) {
        switch (bRequest) {
        case HID_REQ_GetReport:
            // fall-through
        case HID_REQ_GetIdle:
            // fall-through
        case HID_REQ_GetProtocol:
            // fall-through
        default:
            // Unexpected -> STALL
            USBD_SET_EP_STALL(EP0);
            USBD_SET_EP_STALL(EP1);
            break;
        }
    } else {
        switch (bRequest) {
        case HID_REQ_SetReport:
            if (bReportType == 2) { // bReportType.Output
                is_set_report = 1;
                uint8_t len = buf[6]; // wLength [7:0]
                // we expect len only upto 63
                // USBD_SET_DATA1(EP1);
                USBD_SET_PAYLOAD_LEN(EP1, len);
                USBD_PrepareCtrlIn(0, 0);
            } else {
                // Unexpected -> STALL
                USBD_SET_EP_STALL(EP0);
                USBD_SET_EP_STALL(EP1);
            }
            break;
        case HID_REQ_SetIdle:
            USBD_SET_DATA1(EP0);
            USBD_SET_PAYLOAD_LEN(EP0, 0);
            break;
        case HID_REQ_SetProtocol:
            USBD_SET_DATA1(EP0);
            USBD_SET_PAYLOAD_LEN(EP0, 0);
            break;
        default:
            // Unexpected -> STALL
            USBD_SET_EP_STALL(EP0);
            USBD_SET_EP_STALL(EP1);
            break;
        }
    }
}

static void Process_HID_SetReport_Payload(int ep) {
    uint32_t len = USBD_GET_PAYLOAD_LEN(ep);
    uint8_t *buf = (uint8_t *)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(ep);
#ifdef KEYBOARD_SHARED_EP
    if (len == 2) {
        keyboard_led_stats = buf[1];
    }
#else
    if (len == 1) {
        keyboard_led_stats = buf[0];
    }
#endif
}

static void Fill_usbdInfo_with_descriptors(void) {
    get_usb_descriptor(DTYPE_Device << 8, 0, (const void **)&usbdInfo.gu8DevDesc);
    get_usb_descriptor(DTYPE_Configuration << 8, 0, (const void **)&usbdInfo.gu8ConfigDesc);
    
    get_usb_descriptor((DTYPE_String << 8) | 0, 0, (const void **)&(gpu8UsbString[0]));
    get_usb_descriptor((DTYPE_String << 8) | 1, 0, (const void **)&(gpu8UsbString[1]));
    get_usb_descriptor((DTYPE_String << 8) | 2, 0, (const void **)&(gpu8UsbString[2]));
    get_usb_descriptor((DTYPE_String << 8) | 3, 0, (const void **)&(gpu8UsbString[3]));
    usbdInfo.gu8StringDesc = gpu8UsbString;

    gu32UsbHidReportLen[0]=get_usb_descriptor(HID_DTYPE_Report << 8, 0, (const void **)&(gpu8UsbHidReport[0]));
    gu32UsbHidReportLen[1]=get_usb_descriptor(HID_DTYPE_Report << 8, 1, (const void **)&(gpu8UsbHidReport[1]));
    gu32UsbHidReportLen[2]=get_usb_descriptor(HID_DTYPE_Report << 8, 2, (const void **)&(gpu8UsbHidReport[2]));
    gu32UsbHidReportLen[3]=get_usb_descriptor(HID_DTYPE_Report << 8, 3, (const void **)&(gpu8UsbHidReport[3]));

    const void *addr;
    get_usb_descriptor(HID_DTYPE_HID << 8, 0, &addr);
    gu32ConfigHidDescIdx[0] = (uint32_t)((const uint8_t *)addr - (const uint8_t *)usbdInfo.gu8ConfigDesc);
    get_usb_descriptor(HID_DTYPE_HID << 8, 1, &addr);
    gu32ConfigHidDescIdx[1] = (uint32_t)((const uint8_t *)addr - (const uint8_t *)usbdInfo.gu8ConfigDesc);
    get_usb_descriptor(HID_DTYPE_HID << 8, 2, &addr);
    gu32ConfigHidDescIdx[2] = (uint32_t)((const uint8_t *)addr - (const uint8_t *)usbdInfo.gu8ConfigDesc);
    get_usb_descriptor(HID_DTYPE_HID << 8, 3, &addr);
    gu32ConfigHidDescIdx[3] = (uint32_t)((const uint8_t *)addr - (const uint8_t *)usbdInfo.gu8ConfigDesc);
    
    usbdInfo.gu8HidReportDesc = gpu8UsbHidReport;
    usbdInfo.gu32HidReportSize = gu32UsbHidReportLen;
    usbdInfo.gu32ConfigHidDescIdx = gu32ConfigHidDescIdx;

#ifdef SUPPORT_LPM
    usbdInfo.gu8BosDesc = gu8UsbBosDecs;
#endif
}

static void SetConfigCallback(void) {
    configured = 1;
}

bool USBD_HID_isConfigured(void) {
    return configured;
}

void USBD_HID_Start(void) {
    Fill_usbdInfo_with_descriptors();
    USBD_Open(&usbdInfo, Process_ClassRequest, NULL);

    Configure_Endpoints(); // Configure USBD endpoints
    USBD_SetConfigCallback(SetConfigCallback);
    USBD_Start();
}

bool USBD_HID_isSuspended(void) {
    return suspended;
}

void USBD_HID_RemoteWakeup(void) {
    if (!suspended) {
	return;
    }
    USBD_ENABLE_USB(); // This will also enable PHY.
    USBD->ATTR |= USBD_ATTR_RWAKEUP_Msk;
    CLK_SysTickDelay(2000);
    USBD->ATTR &= ~USBD_ATTR_RWAKEUP_Msk;
    USBD_DISABLE_PHY();
}
