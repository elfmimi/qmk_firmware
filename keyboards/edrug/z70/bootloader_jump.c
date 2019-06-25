#include "print.h"
#include "NUC123.h"
#include "hal.h"
#if 0
#include "fmc-min.h"

#define WK_BOARD_DB_FMC_BASE            (0xC000)

#define CONFIG0_CBS_APROM 0x80
#define CONFIG0_CBS_LDROM 0x00
#define CONFIG0_CBS_IAP_ENABSE 0x00
#define CONFIG0_CBS_IAP_DISABLE 0x40
#define CONFIG0_CBS_Msk 0xC0
#define CONFIG0_DFEN_Msk 0x01
#endif

#define SYS_RSTSRC_RSTS_ALL_Msk 0xBF

#if 0
void SetConfigBootSource(int32_t config_boot_source) {
    uint32_t config[2] = {0};

    // SYS_UnlockReg();
    UNLOCKREG();

    // FMC_Open();
    FMC->ISPCON |=  FMC_ISPCON_ISPEN_Msk;
    // FMC_EnableConfigUpdate();
    FMC->ISPCON |= FMC_ISPCON_CFGUEN_Msk;

    // FMC_ReadConfig(config, 2);
    config[0] = FMC_Read(FMC_CONFIG_BASE + 0);
    config[1] = FMC_Read(FMC_CONFIG_BASE + 4);

    config[0] &= ~(CONFIG0_CBS_Msk);
    config[0] |=  (config_boot_source | CONFIG0_CBS_IAP_DISABLE);
    config[1]  = WK_BOARD_DB_FMC_BASE;

    FMC_Erase(FMC_CONFIG_BASE);
    // FMC_WriteConfig(config, 2);
    FMC_Write(FMC_CONFIG_BASE + 0, config[0]);
    FMC_Write(FMC_CONFIG_BASE + 4, config[1]);

    // FMC_DisableConfigUpdate();
    FMC->ISPCON &= ~FMC_ISPCON_CFGUEN_Msk;
    // FMC_Close();
    FMC->ISPCON &= ~FMC_ISPCON_ISPEN_Msk;

    // SYS_LockReg();
    LOCKREG();
}
#endif

void wk_mcu_enter_bl_mode(void)
{
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

void __early_init(void)
{
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
	wk_mcu_enter_bl_mode();
    } else {
	// SetConfigBootSource(CONFIG0_CBS_APROM);
    }
}
