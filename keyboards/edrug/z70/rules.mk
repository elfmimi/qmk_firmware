# project specific files
SRC =	matrix.c \
	z70_led.c \
	bootloader_jump.c

# OPT_DEFS =
MCU_LDSCRIPT = NUC123LD4
BOARD = NUC123

## chip/board settings
# the next two should match the directories in
#  <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = NUMICRO
MCU_SERIES = NUC123
# linker script to use
# it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#  or <this_dir>/ld/
# startup code to use
# is should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
MCU_STARTUP = nuc123
# it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
# Cortex version
MCU  = cortex-m0
# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 6

CUSTOM_MATRIX=yes
EXTRAKEY_ENABLE=yes
DEBOUNCE_TYPE=eager_pk
# eager_pk requires malloc and this is required for malloc
EXTRALDFLAGS += -specs=nosys.specs

# Debug commands enabeled by COMMAND_ENABLE is a set of hot-keys.
# Press IS_COMMAND() + 'H' to show help message.
# IS_COMMAND() is defined in config.h . It is typically LSHIFT + RSHIFT.
COMMAND_ENABLE=yes
# for now, you have to comment out the following line in tmk_core/common.mk
# -> # TMK_COMMON_DEFS += -DNO_PRINT
# to enable output of Debug commands other than LSHIFT + RSHIFT + X.

# Following 5 features are not supported for now.
# CONSOLE_ENABLE=no
# MIDI_ENABLE=no
# RAW_ENABLE=no
# STENO_ENABLE=no
# VIRTSER_ENABLE=no
