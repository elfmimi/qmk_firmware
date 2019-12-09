# QMK Firmware for Z70 / Z70 Pro

[![Development Branch](https://img.shields.io/badge/development%20branch-edrug-blue.svg)](../../tree/edrug)
[![Current Version](https://img.shields.io/github/tag-pre/elfmimi/qmk_firmware.svg)](../../tags)
[![Discord](https://img.shields.io/discord/410252082034114578.svg)](https://discord.gg/nMVjctM)

This is a keyboard firmware based on [qmk firmware](https://github.com/qmk/qmk_firmware) for Z70 and Z70 Pro which use Nuvoton NUC123LD4AN as their MPUs.

## Documentation

Main development branch for Z70 / Z70 Pro is [edrug](../../tree/edrug).

Former edrug-safemode branch was promoted to edrug branch.

Now RGB_MATRIX feature has been properly enabled. 

See also [ChibiOS Contrib](https://github.com/elfmimi/ChibiOS-Contrib)

## Supported Keyboards

* [Edrug Z70 / Z70 Pro](../../tree/edrug/keyboards/edrug/z70/)

## How to build
Do as the following:
* prepare msys2 (mingw64) or cygwin(cygwin64) environment

   or you may use WSL and/or your favorite distribution of Linux
* if using Windows, prepare gnu c compiler provided by ARM

   https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/  
   known good version is 7-2018-q2-update : gcc-arm-none-eabi-7-2017-q4-major-win32.zip
* put the path to the compiler bin in the PATH environmental variable
* git clone https://github.com/elfmimi/qmk_firmware.git qmk_edrug_z70
* cd qmk_edrug_z70
* git checkout edrug
* git submodule update --init lib/chibios lib/chibios-contrib
* if using Linux, inspect util/linux_intall.sh and execute it if you think it's apropriate
* make edrug/z70:default
* you will get **edrug_z70_default.bin** and **.build/edrug_z70_default.elf**
* to boost the speed of make, add -j16 to the end the command line.

## Maintainers

QMK is developed and maintained by [Jack Humbert](https://github.com/jackhumbert) of OLKB with contributions from the community, and of course, [Hasu](https://github.com/tmk). The Z70/Z70 Pro community firmwares are maintained by [Ein Terakawa](https://github.com/elfmimi).
