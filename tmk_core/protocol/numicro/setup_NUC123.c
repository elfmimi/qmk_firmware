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

#define CORE_CLOCK_HXT 72000000

__attribute__((weak))
void setup_CoreClock(void) {
    /* Internal RC 22.1184MHz */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* External XT 12MHz */
    SYS->GPF_MFP |= (SYS_GPF_MFP_PF1_XT1_IN | SYS_GPF_MFP_PF0_XT1_OUT);
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Setup PLL and switch core clock */
    CLK_SetCoreClock(CORE_CLOCK_HXT);
}

__attribute__((weak))
void setup_ModuleClock(void) {
    /* UART0 , UART1 module clock */
    // UART0 and UART1 share same clock.
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_EnableModuleClock(UART0_MODULE);

    /* TIMER0 module clock */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HCLK, 0);
    CLK_EnableModuleClock(TMR0_MODULE);

    /* I2C0 module clock */
    CLK_EnableModuleClock(I2C0_MODULE);

    /* PLL clock : CORE_CLOCK_HXT * 2 , USBD module clock : 72MHz * 2 / 3 = 48MHz */
    CLK_SetModuleClock(USBD_MODULE, 0, CLK_CLKDIV_USB(3));
    CLK_EnableModuleClock(USBD_MODULE);
}

__attribute__((weak))
void setup_MultiFunctionPins(void) {
    // Caution: NUC123xxxAEx need different implementation.

    /* Assign UART0 TXD and RXD on PB1 and PB0 */
    // SYS->GPB_MFP |= (SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB0_UART0_RXD);

    /* Assign UART0 TXD and RXD on PC5 and PC4 */
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC5_UART0_TXD | SYS_GPC_MFP_PC4_UART0_RXD);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC5_UART0_TXD | SYS_ALT_MFP_PC4_UART0_RXD);

    /* Assign I2C0 SCL and SDA on PF3 and PF2 */
    SYS->GPF_MFP |= (SYS_GPF_MFP_PF3_I2C0_SCL | SYS_GPF_MFP_PF2_I2C0_SDA);
    SYS->ALT_MFP1 &= ~(SYS_ALT_MFP1_PF3_Msk | SYS_ALT_MFP1_PF2_Msk);
    SYS->ALT_MFP1 |= (SYS_ALT_MFP1_PF3_I2C0_SCL | SYS_ALT_MFP1_PF2_I2C0_SDA);
}

__attribute__((weak))
void setup_UART0(void) {
    SYS_ResetModule(UART0_RST);
    UART_Open(UART0, 115200);
}

__attribute__((weak))
void setup_UART1(void) {
    SYS_ResetModule(UART1_RST);
    UART_Open(UART1, 115200);
}

volatile uint64_t ms_clk;

// TIMER0_IRQHandler
__attribute__((weak))
void TMR0_IRQHandler(void) {
    ms_clk += 1;
    TIMER_ClearIntFlag(TIMER0);
}

__attribute__((weak))
void setup_TIMER0(void) {
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);

    NVIC_EnableIRQ(TMR0_IRQn);

    TIMER_Start(TIMER0);
}

__attribute__((weak))
void setup_System(void) {
    SYS_UnlockReg();
    setup_CoreClock();
    setup_ModuleClock();
    setup_MultiFunctionPins();
#ifndef NO_DEBUG_PORT
    if (DEBUG_PORT == UART0) {
        setup_UART0();
    }
    if (DEBUG_PORT == UART1) {
        setup_UART1();
    }
#endif
    setup_TIMER0();
    SYS_LockReg();
}
