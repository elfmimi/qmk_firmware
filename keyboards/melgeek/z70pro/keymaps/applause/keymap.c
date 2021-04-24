/*
Copyright 2019 Ein Terakawa <applause@elfmimi.jp>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#ifdef RGB_MATRIX_ENABLE
#include "rgb_matrix.h"
#endif

#define SCTL(kc) (QK_LCTL | QK_LSFT | (kc))
#define INSERT LSFT(KC_INS)

enum preonic_keycodes {
  QWERTY = SAFE_RANGE,
  COLEMAK,
  DVORAK,
  LOWER,
  RAISE,
};

#define LAYOUT LAYOUT_split_70

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  LAYOUT(
KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSLS, KC_GRV, KC_HOME,\
KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSPC, KC_PGUP, \
KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_PGDN, \
KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_END, \
MO(1), KC_LGUI, KC_LALT, KC_SPC, KC_INT4, KC_RALT, MO(2), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT \
	 ),
  LAYOUT(
KC_LGUI, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_INS, KC_DEL, KC_PSCR,\
KC_APP, _______, _______, _______, RESET,   _______, INSERT,  KC_HOME, KC_PGUP, KC_PGDN, KC_END, SCTL(KC_2), LSFT(KC_INS), _______, RGB_RMOD, \
_______, _______, _______, _______, _______, _______, KC_BSPC, KC_UP, KC_LEFT, KC_RIGHT, KC_DOWN, _______, _______, RGB_MOD, \
_______, _______, _______, _______, KC_BTN1, KC_BTN3, KC_BTN2, KC_MS_U, KC_MS_L, KC_MS_R, KC_MS_D, _______, KC_VOLU, KC_PAUS, \
_______, _______, _______, KC_DEL, KC_ENT, KC_CLCK, KC_NLCK, KC_SLCK, RGB_TOG, KC_VOLD, KC_MPLY \
	 ),
  LAYOUT(
KC_LGUI, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_INS, KC_DEL, KC_PSCR,\
KC_APP, _______, _______, _______, RESET,   _______, INSERT,  KC_HOME, KC_PGUP, KC_PGDN, KC_END, SCTL(KC_2), LSFT(KC_INS), _______, RGB_SPI, \
_______, _______, _______, _______, _______, _______, KC_BSPC, KC_UP, KC_LEFT, KC_RIGHT, KC_DOWN, _______, _______, RGB_SPD, \
_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_VAI, KC_PAUS, \
_______, _______, _______, _______, _______, _______, MO(2), _______, RGB_HUD, RGB_VAD, RGB_HUI \
	 )
};

const uint16_t fn_actions[] = {
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#if 0
  switch (keycode) {
        case QWERTY:
          if (record->event.pressed) {
            set_single_persistent_default_layer(_QWERTY);
          }
          return false;
          break;
        case COLEMAK:
          if (record->event.pressed) {
            set_single_persistent_default_layer(_COLEMAK);
          }
          return false;
          break;
        case DVORAK:
          if (record->event.pressed) {
            set_single_persistent_default_layer(_DVORAK);
          }
          return false;
          break;
        case LOWER:
          if (record->event.pressed) {
            layer_on(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
          } else {
            layer_off(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
          }
          return false;
          break;
        case RAISE:
          if (record->event.pressed) {
            layer_on(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
          } else {
            layer_off(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
          }
          return false;
          break;
        case BACKLIT:
          if (record->event.pressed) {
            register_code(KC_RSFT);
            #ifdef BACKLIGHT_ENABLE
              backlight_step();
            #endif
            #ifdef __AVR__
            PORTE &= ~(1<<6);
            #endif
          } else {
            unregister_code(KC_RSFT);
            #ifdef __AVR__
            PORTE |= (1<<6);
            #endif
          }
          return false;
          break;
      }
#endif
    return true;
};

uint32_t last_time;
int rgb_anim_idx;
void matrix_init_user(void) {
#ifdef RGB_MATRIX_ENABLE
    rgb_anim_idx = RGB_MATRIX_CYCLE_ALL;
    // rgb_matrix_mode_noeeprom(rgb_anim_idx);
     // rgb_matrix_mode_noeeprom(RGB_MATRIX_MULTISPLASH);
    last_time = timer_read32();
#endif
}

#ifdef RGB_MATRIX_ENABLE
void matrix_scan_user(void) {
#if 0
    uint32_t now = timer_read32();
    if (TIMER_DIFF_32(now, last_time) >= 1500) {
	last_time = now;
	rgb_anim_idx = (rgb_anim_idx - RGB_MATRIX_CYCLE_ALL + 1) % 11 + RGB_MATRIX_CYCLE_ALL;
	rgb_matrix_mode_noeeprom(rgb_anim_idx);
    }
#endif
}
#endif
