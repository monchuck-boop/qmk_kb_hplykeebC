// Copyright 2021 @Yowkees
// Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)
// Copyright 2026 Idan Kamara (@idank)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define TAP_CODE_DELAY 5

// Auto mouse layer: pointer motion temporarily activates layer 2, which carries
// the mouse buttons, as in the stock Keyball keymap. Pointing/scroll behavior
// comes from the holykeebs userspace.
#define POINTING_DEVICE_AUTO_MOUSE_ENABLE
#define AUTO_MOUSE_DEFAULT_LAYER 2

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// Vial identity and unlock combo. VIAL_INSECURE lives in the shared userspace
// config (users/holykeebs/config.h), so it is intentionally not repeated here.
// The unlock combo is unused while VIAL_INSECURE is set, but kept so re-securing
// later doesn't require re-picking keys: the two outermost top keys, matrix
// [0,0] (top-left of the left half) and [5,0] (top-right of the right half).
#define VIAL_KEYBOARD_UID {0xD7, 0x48, 0x4C, 0x7C, 0xE4, 0xBD, 0x28, 0xD5}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 5 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 0 }


// データ不正エラーを防ぐダミーピンと、内部メモリ(EEPROM)左右判別をここにも明示する
#define SPLIT_HAND_PIN GP29
#define EE_HANDS
