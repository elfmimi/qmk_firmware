#!/bin/bash
make -r -R -C ./ -f build_keyboard.mk  KEYBOARD=angel64/rev1 KEYMAP=default SILENT=false $*
