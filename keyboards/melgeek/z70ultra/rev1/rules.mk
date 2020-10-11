# MCU name
MCU = STM32F303

# in case without RGB_MATRIX *notice the use of lazy evaluation, or deffered expansion.
OPT_DEFS += $(if $(filter no,$(RGB_MATRIX_ENABLE)),-DIS31FL3741)
SRC += $(if $(filter no,$(RGB_MATRIX_ENABLE)),is31fl3741.c)
QUANTUM_LIB_SRC += $(if $(filter no,$(RGB_MATRIX_ENABLE)),i2c_master.c)
# COMMON_VPATH is defined as a simple variable, which means 'immediate' not 'deffered' .
# COMMON_VPATH += $(if $(filter no,$(RGB_MATRIX_ENABLE)),$(DRIVER_PATH)/issi)
VPATH += $(if $(filter no,$(RGB_MATRIX_ENABLE)),$(DRIVER_PATH)/issi)

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = lite        # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes          # Mouse keys
EXTRAKEY_ENABLE = yes          # Audio control and System control
CONSOLE_ENABLE = no            # Console for debug
COMMAND_ENABLE = no            # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no          # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = yes              # USB Nkey Rollover
BACKLIGHT_ENABLE = no          # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no           # Enable keyboard RGB underglow
BLUETOOTH_ENABLE = no          # Enable Bluetooth
AUDIO_ENABLE = no              # Audio output
RGB_MATRIX_ENABLE = IS31FL3741 # Use RGB matrix
NO_USB_STARTUP_CHECK = no      # Disable initialization only when usb is plugged in

LAYOUTS = 65_ansi_blocker
