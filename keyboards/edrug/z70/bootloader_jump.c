#include "print.h"
#include "NUC123.h"
#include "hal.h"

#define SYS_RSTSRC_RSTS_ALL_Msk 0xBF

void enter_bootloader_mode(void) {
    // SetConfigBootSource(CONFIG0_CBS_LDROM);

    SYS->RSTSRC = SYS_RSTSRC_RSTS_ALL_Msk;

    // SYS_UnlockReg();
    UNLOCKREG();

    FMC->ISPCON |= 0x03;
    SYS->IPRSTC1 |= SYS_IPRSTC1_CHIP_RST_Msk;

    /* NEVER HERE */

    // SYS_LockReg();
    LOCKREG();
}

void bootloader_jump(void) {
    // Issue CPU reset
    // SYS_UnlockReg();
    UNLOCKREG();

    SYS->IPRSTC1 |= SYS_IPRSTC1_CPU_RST_Msk;

    /* NEVER HERE */

    // SYS_LockReg();
    LOCKREG();
}

void __early_init(void) {
    bool enter_bootloader = false;
    // if it is a SYS reset, we are just re-booted from bootloader.
    if (SYS->RSTSRC & SYS_RSTSRC_RSTS_SYS_Msk) {
        SYS->RSTSRC = SYS_RSTSRC_RSTS_SYS_Msk;
    }
    // if it is a CPU reset, assume it was issued by bootloader_jump()
    if (SYS->RSTSRC & SYS_RSTSRC_RSTS_CPU_Msk) {
        SYS->RSTSRC = SYS_RSTSRC_RSTS_CPU_Msk;
        enter_bootloader = true;
    }
    if (!enter_bootloader) {
        palSetPadMode(GPIOB, 8, PAL_MODE_INPUT); // R0
        palSetPadMode(GPIOD, 4, PAL_MODE_OUTPUT_PUSHPULL); // C0
        // wait for a little while
        for (int i = 0; i < 100; i++) {
            palSetPad(GPIOD, 4);
        }

        // Row:0 & Col:0 designate ESC key.
        if (palReadPad(GPIOB, 8)==PAL_HIGH) {
            enter_bootloader = true;
        }
    }
    if (enter_bootloader) {
        enter_bootloader_mode();
    }
}
