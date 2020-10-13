NUMICRO_DIR = protocol/numicro

# Source file with IRQHandler must be excluded from LTO (Link-Time-Optimization) .
SRC += $(NUMICRO_DIR)/startup.c
SRC += $(NUMICRO_DIR)/main_numicro.c
SRC += $(NUMICRO_DIR)/setup_$(MCU_SERIES).c
SRC += $(NUMICRO_DIR)/debugport.c
SRC += $(NUMICRO_DIR)/hardfault.c
SRC += $(NUMICRO_DIR)/usbd_hid.c
SRC += $(NUMICRO_DIR)/driver_numicro.c
ifeq ($(strip $(CONSOLE_ENABLE)), yes)
SRC += $(NUMICRO_DIR)/hid_console.c
endif
SRC += usb_descriptor.c
# eeprom.c is listed in common.mk
#SRC += $(NUMICRO_DIR)/eeprom.c
# Place vectors_XXX.c at the end to allow weak symbols from other source files to take-over.
SRC += $(NUMICRO_DIR)/vectors_$(MCU_SERIES).c
# QUANTUM_LIB_SRC += i2c_master.c
ifneq ($(filter $(QUANTUM_LIB_SRC),i2c_master.c),)
QUANTUM_LIB_SRC += hal_i2c.c
QUANTUM_LIB_SRC += hal_i2c_lld.c
endif

ifeq ($(MCU_SERIES),NUC123)
# SRC += $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/NUC123/Source/gcc/startup_NUC123.S
SRC += $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/NUC123/Source/system_NUC123.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/sys.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/clk.c
#SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/fmc.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/gpio.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/timer.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/uart.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/i2c.c
SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/usbd.c
#SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/pdma.c
#SRC += $(NUMICRO_BSP_DIR)/Library/StdDriver/src/retarget.c
endif

# Search Path
# using COMMON_VPATH will add items before VPATH of ../numicro.mk
COMMON_VPATH += $(TMK_DIR)/$(NUMICRO_DIR)
# VPATH += $(TMK_DIR)/$(NUMICRO_DIR)
# borrow lufa_utils/LUFA/Drivers/USB/USB.h from chibios
# need this for #include <LUFA/Drivers/USB/USB.h> in usb_descriptor.h
CHIBIOS_DIR = protocol/chibios
VPATH += $(TMK_PATH)/$(CHIBIOS_DIR)/lufa_utils
