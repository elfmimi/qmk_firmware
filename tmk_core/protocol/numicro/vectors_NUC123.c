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

/* Interrupt vector table re-written in C to support LTO (Link-Time-Optimization). */

void __stack(void);
void Reset_Handler(void);

#define DefineDefaultHandler(x) void x(void) __attribute__((weak, alias("Default_Handler"), __noreturn__))
DefineDefaultHandler(NMI_Handler);
DefineDefaultHandler(HardFault_Handler);
DefineDefaultHandler(SVC_Handler);
DefineDefaultHandler(PendSV_Handler);
DefineDefaultHandler(SysTick_Handler);

DefineDefaultHandler(BOD_IRQHandler);
DefineDefaultHandler(WDT_IRQHandler);
DefineDefaultHandler(EINT0_IRQHandler);
DefineDefaultHandler(EINT1_IRQHandler);
DefineDefaultHandler(GPAB_IRQHandler);
DefineDefaultHandler(GPCDF_IRQHandler);
DefineDefaultHandler(PWMA_IRQHandler);
DefineDefaultHandler(TMR0_IRQHandler);
DefineDefaultHandler(TMR1_IRQHandler);
DefineDefaultHandler(TMR2_IRQHandler);
DefineDefaultHandler(TMR3_IRQHandler);
DefineDefaultHandler(UART0_IRQHandler);
DefineDefaultHandler(UART1_IRQHandler);
DefineDefaultHandler(SPI0_IRQHandler);
DefineDefaultHandler(SPI1_IRQHandler);
DefineDefaultHandler(SPI2_IRQHandler);
DefineDefaultHandler(SPI3_IRQHandler);
DefineDefaultHandler(I2C0_IRQHandler);
DefineDefaultHandler(I2C1_IRQHandler);
DefineDefaultHandler(CAN0_IRQHandler);
DefineDefaultHandler(CAN1_IRQHandler);
DefineDefaultHandler(SC012_IRQHandler);
DefineDefaultHandler(USBD_IRQHandler);
DefineDefaultHandler(PS2_IRQHandler);
DefineDefaultHandler(ACMP_IRQHandler);
DefineDefaultHandler(PDMA_IRQHandler);
DefineDefaultHandler(I2S_IRQHandler);
DefineDefaultHandler(PWRWU_IRQHandler);
DefineDefaultHandler(ADC_IRQHandler);
DefineDefaultHandler(CLKDIRC_IRQHandler);
DefineDefaultHandler(RTC_IRQHandler);

typedef void (*handler)(void);
__attribute__ ((used, section(".vectors")))
handler __Vectors[] = {
    __stack,
    Reset_Handler,
    NMI_Handler, // NonMaskableInt_IRQn = -14
    HardFault_Handler, // HardFault_IRQn = -13
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    SVC_Handler, // SVCall_IRQn = -5
    0,
    0,
    PendSV_Handler, // PendSV_IRQn = -2
    SysTick_Handler, // SysTick_IRQn = -1
    
    // address = 0x40 , index = 0x10 , IRQn = 0
    BOD_IRQHandler,
    WDT_IRQHandler,
    EINT0_IRQHandler,
    EINT1_IRQHandler,
    GPAB_IRQHandler,
    GPCDF_IRQHandler,
    PWMA_IRQHandler,
    0,

    // address = 0x60 , index = 0x18 , IRQn = 8
    TMR0_IRQHandler,
    TMR1_IRQHandler,
    TMR2_IRQHandler,
    TMR3_IRQHandler,
    UART0_IRQHandler,
    UART1_IRQHandler,
    SPI0_IRQHandler,
    SPI1_IRQHandler,

    // address = 0x80 , index = 0x20 , IRQn = 16
    SPI2_IRQHandler,
    SPI3_IRQHandler,
    I2C0_IRQHandler,
    I2C1_IRQHandler,
    CAN0_IRQHandler,
    CAN1_IRQHandler,
    SC012_IRQHandler,
    USBD_IRQHandler,

    // address = 0xA0 , index = 0x28 , IRQn = 24
    PS2_IRQHandler,
    ACMP_IRQHandler,
    PDMA_IRQHandler,
    I2S_IRQHandler,
    PWRWU_IRQHandler,
    ADC_IRQHandler,
    0,
    RTC_IRQHandler,
};

__attribute__ ((__noreturn__))
void Default_Handler(void) { do {} while (1); }

