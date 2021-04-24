# project specific files
SRC += matrix.c
SRC += led.c

# with RGB_MATRIX
SRC += $(if $(filter IS31FL3741,$(RGB_MATRIX_ENABLE)),rgb.c)

# without RGB_MATRIX
OPT_DEFS += $(if $(filter no,$(RGB_MATRIX_ENABLE)),-DIS31FL3741)
SRC += $(if $(filter no,$(RGB_MATRIX_ENABLE)),is31fl3741.c)
QUANTUM_LIB_SRC += $(if $(filter no,$(RGB_MATRIX_ENABLE)),i2c_master.c)
# COMMON_VPATH is defined as a simple variable, which means 'immediate' not 'deffered' .
# COMMON_VPATH += $(if $(filter no,$(RGB_MATRIX_ENABLE)),$(DRIVER_PATH)/issi)
VPATH += $(if $(filter no,$(RGB_MATRIX_ENABLE)),$(DRIVER_PATH)/issi)

NUMICRO = NUC123LD4AN0
# OPT_DEFS =
#MCU_LDSCRIPT = NUC123LD4
#BOARD = NUC123

## chip/board settings
# the next two should match the directories in
#  <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
#MCU_FAMILY = NUMICRO
#MCU_SERIES = NUC123
# linker script to use
# it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#  or <this_dir>/ld/
# startup code to use
# is should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
#MCU_STARTUP = nuc123
# it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
# Cortex version
MCU  = cortex-m0
# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
#ARMV = 6

CUSTOM_MATRIX=lite
EXTRAKEY_ENABLE=yes
MOUSEKEY_ENABLE=yes
DEBOUNCE_TYPE=eager_pk

# Debug commands enabeled by COMMAND_ENABLE is a set of hot-keys.
# Press IS_COMMAND() + 'H' to show help message.
# IS_COMMAND() is defined in config.h . It is typically LSHIFT + RSHIFT.
COMMAND_ENABLE=yes
# In case you don't use CONSOLE_ENABLE,
# you have to comment out the following line in tmk_core/common.mk
# -> # TMK_COMMON_DEFS += -DNO_PRINT
# to enable output of Debug commands other than LSHIFT + RSHIFT + X.

BOOTMAGIC_ENABLE=yes
CONSOLE_ENABLE=yes
RGB_MATRIX_ENABLE=IS31FL3741
DEBUG_PORT=UART0

# Following 4 features are not supported for now.
# MIDI_ENABLE=no
# RAW_ENABLE=no
# STENO_ENABLE=no
# VIRTSER_ENABLE=no

# STDOUT_ENABLE=yes
CFLAGS += -DZ70PRO_PCB_SPLIT_70
