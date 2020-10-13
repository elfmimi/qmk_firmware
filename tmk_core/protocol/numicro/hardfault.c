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
#include "printf.h"  // lib/printf/printf.h

#ifdef BUFFERED_DEBUG_PORT
void debugport_wait_buffer_empty(void); // defined in debugport.c
#endif

__attribute__ ((noinline, __noreturn__))
static void _HardFault_Handler(uint32_t stack[]) {
    printf_("HardFault_Handler() is called.\n");

    const char *const reg_names[] = { "r0", "r1", "r2", "r3", "r12", "lr", "pc", "psr" };
    for (int i = 0; i < 8; i++) {
        printf_("%-3s = 0x%08X\n", reg_names[i], stack[i]);
    }

#ifdef BUFFERED_DEBUG_PORT
    // Wait until the buffer clears.
    debugport_wait_buffer_empty();
#endif

    // You may want to activate breakpoint here.
#if 1
    printf_("Invoking breakpoint instrution.\n");
#ifdef BUFFERED_DEBUG_PORT
    // Wait until the buffer clears.
    debugport_wait_buffer_empty();
#endif
    asm volatile("bkpt");
#endif

    do { } while(1);
}

__attribute__ ((naked, __noreturn__))
void HardFault_Handler (void) {
    // We are yet to inplement process mode theads, so
    // it is safe to assume that SP_main aka MSP is always active.
    asm volatile("mrs r0, msp\n\t" "bx %0" : : "r" (_HardFault_Handler) : "r0");
}
