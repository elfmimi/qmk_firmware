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

#ifdef NO_PRINT
#include "printf.h"  // lib/printf/printf.h
#endif

#ifndef NO_LIBC_STDOUT
#include "stdio.h"
#endif

void setup_System(void);

void main_subtasks(void);

#ifdef CONSOLE_ENABLE
void console_task(void);
void sendchar_console(uint8_t c);
#endif /* CONSOLE_ENABLE */

#ifndef NO_DEBUG_PORT
void sendchar_debugport(uint8_t ch); // defined in debugport.c as SendChar_ToUART
#endif
#ifdef BUFFERED_DEBUG_PORT
void debugport_fill_fifo(void);
#endif

void eeprom_init(void); // This is defined in eeprom.c .

// for printf_
void _putchar(char c) {
#ifndef NO_DEBUG_PORT
    sendchar_debugport(c);
#endif
#ifdef CONSOLE_ENABLE
    sendchar_console(c);
#endif /* CONSOLE_ENABLE */
}

#ifdef NO_LIBC_STDOUT
// If you really need to use printf(), puts(), putc(), putchar() and so on,
// switch implementation of _write() to the other one shown below.
extern int Dont_use_stdio_functions(int fd, char *ptr, int len);
int _write(int fd, char *ptr, int len) {
    return Dont_use_stdio_functions(fd, ptr, len);
}
#else
int _write(int fd, char *ptr, int len) {
    for (int i=len; i; i--) {
        _putchar(*ptr++);
    }
    return len;
}
#endif

extern host_driver_t numicro_driver;

void main_subtasks(void) {
    // main_subtask_usb_state();
    // main_subtask_power_check();
    // main_subtask_usb_extra_device();

#ifdef CONSOLE_ENABLE
    console_task();
#endif /* CONSOLE_ENABLE */

#ifdef BUFFERED_DEBUG_PORT
    debugport_fill_fifo();
#endif /* BUFFERED_DEBUG_PORT */
}


int main(void) {
    setup_System();

#ifndef NO_LIBC_STDOUT
    // No buffering at newlib/llibc level when using printf(), putchar(), and so on .
    setbuf(stdout, NULL);
#endif

#if !defined(NO_DEBUG_PORT) || defined(CONSOLE_ENABLE)
    // Let these be active even with NO_PRINT.
    printf_("\n\n" "PLATFORM=NUMICRO\n");
    printf_(__FILE__": main()\n");
    // printf_("PLLClock = %u\n", CLK_GetPLLClockFreq());
    printf_("CoreClock = %u\n", SystemCoreClock);

    eeprom_init();

#if !defined(NO_PRINT) && !defined(NO_DEBUG)
    //    debug_enable = true;
    //    debug_matrix = true;
    //    debug_keyboard = true;
    //    debug_mouse = true;
#endif
#endif

    USBD_HID_Start();

    /* Enable USB device interrupt */
    NVIC_EnableIRQ(USBD_IRQn); // Enable USBD_IRQHandler()

#if 0
    /* start to IN data */
    g_u8EPisBusy[EP2] = 0;
#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
    g_u8EPisBusy[EP3] = 0;
#endif
#endif

    // matrix_init();

    keyboard_setup();

    // keyboard_init() will call bootmagic() and initialises debug_config
    keyboard_init();

#if !defined(NO_PRINT) && !defined(NO_DEBUG)
    // override settings from eeprom for ease of testing
    //    debug_enable = true;
    //    debug_matrix = true;
    //    debug_keyboard = true;
    //    debug_mouse = true;
    // dprint() , dprintln() and dprintf() are activated here after
#endif

    host_set_driver(&numicro_driver);

    while (1) {
        main_subtasks();  // Note these tasks will also be run while waiting for USB keyboard polling intervals

        keyboard_task();

	bool flag = false;
	while (USBD_HID_isSuspended()) {
	    if (!flag) {
		flag = true;
		printf("SUSPENDED\n");
	    }
	    CLK_SysTickDelay(10000);
	    if (suspend_wakeup_condition()) {
		// Remote WakeUP
		printf("RemoteWakeup\n");
		USBD_HID_RemoteWakeup();
		CLK_SysTickDelay(10000);
	    }
	}
	if (flag) {
	    printf("RESUMED\n");
	}
    }
}
