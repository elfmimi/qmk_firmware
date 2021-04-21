# Z70 Pro and Z70 support

QMK firmware for Z70 Pro and Z70 keyboards.

This firmware automatically distinguish the PCB between Z70 Pro and Z70 .

It seems additional endpoints of USB interface is not being handled correctly.
(* hal_usb_lld.c of NUC123 is the prime suspect *)
Thus CONSOLE_ENABLE and other features depending on it does not work.
If you need the output of printf for debugging you have to rely on UART0-TX,
which is available as a 2-pin header on the PCB. Baudrate is 115200 bps.

EEPROM is not yet supported.
