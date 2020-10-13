# Hey Emacs, this is a -*- makefile -*-
##############################################################################
# Compiler settings
#
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
AR = arm-none-eabi-ar
NM = arm-none-eabi-nm
HEX = $(OBJCOPY) -O $(FORMAT) -R .eeprom -R .fuse -R .lock -R .signature
EEP = $(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT)
BIN = $(OBJCOPY) -O binary
#BIN = $(OBJCOPY) -O binary -R .eeprom -R .fuse -R .lock -R .signature

ifeq ($(NUMICRO),NUC123LD4AN0)
MCU_SERIES=NUC123
MCU_PACKAGE=NUC123L
endif

ifdef NUMICRO_BSP_DIR
ifeq ("$(wildcard $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/$(MCU_SERIES))","")
$(error "NUMICRO_BSP_DIR is defined but couldn't find Nuvoton BSP there.")
endif
else
NUMICRO_BSP_DIR=$(LIB_PATH)/numicro/$(MCU_SERIES)BSP
ifeq ("$(wildcard $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/$(MCU_SERIES))","")
$(error "Couldn't find Nuvoton BSP for the specified MCU.")
endif
endif

# Let's define MCU specific linker scripts later.
LDSCRIPT = $(TOP_DIR)/platforms/numicro/ld/gcc_arm.ld
# LDSCRIPT = $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/$(MCU_SERIES)/Source/gcc/gcc_arm.ld

COMMON_VPATH += $(LIB_PATH)/CMSIS/Core/Include
#COMMON_VPATH += $(NUMICRO_BSP_DIR)/Library/CMSIS/Include
COMMON_VPATH += $(DRIVER_PATH)/numicro
# COMMON_VPATH is added before VPATH of common.mk
# using VPATH will add items after VPATH of common.mk
VPATH += $(NUMICRO_BSP_DIR)/Library/Device/Nuvoton/$(MCU_SERIES)/Include
VPATH += $(NUMICRO_BSP_DIR)/Library/StdDriver/inc

THUMBFLAGS = -DTHUMB_PRESENT -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -mthumb -DTHUMB

COMPILEFLAGS += -funsigned-char
COMPILEFLAGS += -funsigned-bitfields
COMPILEFLAGS += -ffunction-sections
COMPILEFLAGS += -fdata-sections 
#COMPILEFLAGS += -fshort-enums
# USB String Descriptor, which is defined using LSTR() in usb_descriptor.c, need wchar to be 16-bit.
COMPILEFLAGS += -fshort-wchar
#COMPILEFLAGS += -fno-inline-small-functions
COMPILEFLAGS += -fno-strict-aliasing
COMPILEFLAGS += -fno-common 
COMPILEFLAGS += -DCORTEX_USE_FPU=FALSE
#COMPILEFLAGS += -mfloat-abi=hard
#COMPILEFLAGS += -mfpu=fpv4-sp-d16
COMPILEFLAGS += $(THUMBFLAGS)
# see startup_NUC123.S
# As .bss being cleared by _mainCRTStartup(), we don't need this.
# ASFLAGS += -D__STARTUP_CLEAR_BSS
# see startup.c
# As .bss being cleared by _mainCRTStartup(), we don't need this.
# COMPILEFLAGS += -D__STARTUP_CLEAR_BSS
ifdef INIT_SYSCLK_AT_BOOTING
# see system_NUC123.c
COMPILEFLAGS += -DINIT_SYSCLK_AT_BOOTING
else
# see startup_NUC123.S
ASFLAGS += -D__NO_SYSTEM_INIT
endif
ifdef DEBUG_PORT
COMPILEFLAGS += -DDEBUG_PORT=$(strip $(DEBUG_PORT))
COMPILEFLAGS += -DBUFFERED_DEBUG_PORT
else
COMPILEFLAGS += -DNO_DEBUG_PORT
endif
COMPILEFLAGS += -Wformat=0
ifdef NUMICRO_BOARD
COMPILEFLAGS += -DNUMICRO_BOARD_$(NUMICRO_BOARD)
endif
#ALLOW_WARNINGS = yes
ifneq ($(strip $(STDOUT_ENABLE)), yes)
COMPILEFLAGS += -DNO_LIBC_STDOUT
endif

CFLAGS += $(COMPILEFLAGS)

CPPFLAGS += $(COMPILEFLAGS)
CPPFLAGS += -fno-exceptions -std=c++11

ASFLAGS += $(THUMBFLAGS)

# -fshort-wchar causes link time warnings , so this is to surpress that warning.
LDFLAGS += -Wl,--no-wchar-size-warning
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map="%OUT%%PROJ_NAME%.map"
LDFLAGS += -Wl,--start-group
LDFLAGS += -Wl,--end-group
# LDFLAGS += --specs=rdimon.specs
LDFLAGS += -T$(LDSCRIPT)
ifeq (,)
#LDFLAGS += --specs=nano.specs
else
#LDFLAGS += --specs=nosys.specs
#LDFLAGS += -nostartfiles
#ADEFS += -D__START=main
endif

OPT_DEFS += -DPROTOCOL_NUMICRO
# EP0_MAX_PKT_SIZE
OPT_DEFS += -DFIXED_CONTROL_ENDPOINT_SIZE=8
OPT_DEFS += -DFIXED_NUM_CONFIGURATIONS=1

MCUFLAGS = -mcpu=$(MCU)
MCUFLAGS += -D__$(NUMICRO)__
MCUFLAGS += -D__$(MCU_SERIES)__
ifdef MCU_PACKAGE
MCUFLAGS += -D__$(MCU_PACKAGE)__
endif

# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS =

BOOTLOADER ?= nu-isp
NU_ISP_CLI ?= nu-isp-cli

define EXEC_NU_ISP
	until $(NU_ISP_CLI) info | grep -q "PDID"; do\
		printf "$(MSG_BOOTLOADER_NOT_FOUND)" ;\
		sleep 5 ;\
	done
	$(NU_ISP_CLI) $(BUILD_DIR)/$(TARGET).bin
endef

bin: $(BUILD_DIR)/$(TARGET).bin

nu-isp: $(FIRMWARE_FORMAT)
	$(call EXEC_NU_ISP)

flash: $(FIRMWARE_FORMAT)
ifneq ($(strip $(PROGRAM_CMD)),)
	$(PROGRAM_CMD)
else ifeq ($(strip $(BOOTLOADER)),nu-isp)
	$(call EXEC_NU_ISP)
else
	$(PRINT_OK); $(SILENT) || printf "$(MSG_FLASH_BOOTLOADER)"
endif
#	$(PRINT_OK); $(SILENT) || printf "$(MSG_FLASH_ARCH)"
