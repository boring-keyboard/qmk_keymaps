/*
Copyright 2020 <contact@vwolf.be>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

enum layers {
    _LAYER0,
    _LAYER1,
    _LAYER2,
};

enum tapdances{
  TD_CODE_JUMP = 0,
  TD_BACKSPACE_WORD,
  TD_CODE_EQ,
  TD_CODE_MINUS,
  TD_CODE_1,
  TD_CODE_2,
  TD_CODE_3,
  TD_CODE_4,
  TD_CODE_5,
  TD_CODE_6,
  TD_CODE_7,
  TD_CODE_8,
  TD_CODE_9,
  TD_CODE_0,
  TD_CODE_COMM,
  TD_CODE_DOT,
  TD_CODE_SLSH,
  TD_CODE_SCLN,
  TD_CODE_BSLS
};


typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(TD_CODE_EQ):  // list all tap dance keycodes with tap-hold configurations
        case TD(TD_CODE_MINUS):
        case TD(TD_CODE_1):
        case TD(TD_CODE_2):
        case TD(TD_CODE_3):
        case TD(TD_CODE_4):
        case TD(TD_CODE_5):
        case TD(TD_CODE_6):
        case TD(TD_CODE_7):
        case TD(TD_CODE_8):
        case TD(TD_CODE_9):
        case TD(TD_CODE_0):
        case TD(TD_CODE_COMM):
        case TD(TD_CODE_DOT):
        case TD(TD_CODE_SLSH):
        case TD(TD_CODE_SCLN):
        case TD(TD_CODE_BSLS):
        case TD(TD_BACKSPACE_WORD):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

void tap_dance_tap_hold_space_wrap_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            tap_code(KC_SPACE);  // 发送空格
            tap_code(tap_hold->tap);  // 发送 hold 对应的按键
            tap_code(KC_SPACE);  // 再发送空格
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_space_wrap_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}


void tap_dance_tap_hold_brace_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            SEND_STRING("()");
            tap_code(KC_LEFT);
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_brace_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(tap) \
    { .fn = {NULL, tap_dance_tap_hold_space_wrap_finished, tap_dance_tap_hold_space_wrap_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, 0, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_BRACE(tap) \
    { .fn = {NULL, tap_dance_tap_hold_brace_finished, tap_dance_tap_hold_brace_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, 0, 0}), }

tap_dance_action_t tap_dance_actions[] = {
    [TD_CODE_JUMP] = ACTION_TAP_DANCE_DOUBLE(LCTL(KC_MINS), LCTL(LSFT(KC_MINS))),
    // 长按左右添加空格
    [TD_CODE_EQ] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_EQL),
    [TD_CODE_MINUS] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_MINUS),
    [TD_CODE_1] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_1),
    [TD_CODE_2] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_2),
    [TD_CODE_3] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_3),
    [TD_CODE_4] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_4),
    [TD_CODE_5] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_5),
    [TD_CODE_6] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_6),
    [TD_CODE_7] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_7),
    [TD_CODE_8] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_8),
    [TD_CODE_0] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_0),
    [TD_CODE_COMM] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_COMM),
    [TD_CODE_DOT] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_DOT),
    [TD_CODE_SLSH] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_SLSH),
    [TD_CODE_SCLN] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_SCLN),
    [TD_CODE_BSLS] = ACTION_TAP_DANCE_TAP_HOLD_SPACE_WRAP(KC_BSLS),
    // 长按9输入左右括号并移动光标
    [TD_CODE_9] = ACTION_TAP_DANCE_TAP_HOLD_BRACE(KC_9),
    // 长按backsapce删除单词
    [TD_BACKSPACE_WORD] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LALT(KC_BSPC)),
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_LAYER0] = LAYOUT_60_hhkb(
        QK_GESC, TD(TD_CODE_1),    TD(TD_CODE_2),    TD(TD_CODE_3),    TD(TD_CODE_4),    TD(TD_CODE_5),    TD(TD_CODE_6),    TD(TD_CODE_7),    TD(TD_CODE_8),    TD(TD_CODE_9),    TD(TD_CODE_0),    TD(TD_CODE_MINUS), TD(TD_CODE_EQ),  TD(TD_CODE_BSLS), KC_GRV,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, TD(TD_BACKSPACE_WORD),
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    TD(TD_CODE_SCLN), KC_QUOT,          KC_ENT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    TD(TD_CODE_COMM), TD(TD_CODE_DOT),  TD(TD_CODE_SLSH),          KC_RSFT, MO(1),
                 KC_LALT, KC_LGUI,                            KC_SPC,                             KC_RGUI,          TD(TD_CODE_JUMP)
    ),

    [_LAYER1] = LAYOUT_60_hhkb(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_TRNS,
        TG(2), RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, KC_TRNS, KC_TRNS, KC_UP,   KC_TRNS, KC_TRNS,
        KC_CAPS, RGB_SPI, RGB_SPD, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LEFT, KC_RIGHT,         KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DOWN,          KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS,                            KC_TRNS,                            KC_TRNS,          KC_TRNS
    ),

    [_LAYER2] = LAYOUT_60_hhkb(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_MS_BTN1, KC_WH_U, KC_MS_BTN2, KC_TRNS, KC_TRNS, KC_MS_BTN1, KC_MS_UP, KC_MS_BTN2, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_WH_L, KC_WH_D, KC_WH_R, KC_TRNS, KC_TRNS, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, KC_TRNS, KC_TRNS,      KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS,
                 KC_TRNS, KC_TRNS,                            KC_TRNS,                            KC_TRNS,          KC_TRNS
    )
};
