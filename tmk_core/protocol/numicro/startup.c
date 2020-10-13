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

/* Startup chores re-written in C */

typedef unsigned int uint32_t;
extern uint32_t __etext[];
extern uint32_t __data_start__[];
extern uint32_t __data_end__[];

#ifdef __STARTUP_CLEAR_BSS
extern uint32_t __bss_start__[];
extern uint32_t __bss_end__[];
#endif

#ifndef __START
// _start is an alias of _mainCRTStartup .
#define __START _start
#endif
void __START(void);
__attribute__ ((naked))
void Reset_Handler(void) {
    // Copy 
    uint32_t *src = __etext;
    uint32_t *dst = __data_start__;
    while (dst != __data_end__) {
        *dst++ = *src++;
    }

#ifdef __STARTUP_CLEAR_BSS
    // As .bss being cleared by _mainCRTStartup(), we don't need this.
    uint32_t *bss = __bss_start__;
    while (bss != __bss_end__) {
        *bss++ = 0;
    }
#endif

    __START();
    /* _mainCRTStartup() will call exit() so execution will never return here. */
}
