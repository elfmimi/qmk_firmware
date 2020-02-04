#!/bin/bash
make -r -R -C ./ -f build_keyboard.mk  KEYBOARD=thedogkeyboard KEYMAP=default SILENT=false $*
