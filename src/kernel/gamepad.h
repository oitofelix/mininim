/*
  gamepad.h -- gamepad module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MININIM_GAMEPAD_H
#define MININIM_GAMEPAD_H

/* variables */
extern bool flip_gamepad_horizontal, flip_gamepad_vertical;

/* keyboard */
extern ALLEGRO_KEYBOARD_STATE keyboard_state;
extern struct key key;

/* joystick */
extern int joystick_button;
extern ALLEGRO_JOYSTICK *joystick;
extern ALLEGRO_JOYSTICK_STATE joystick_state;
extern float joystick_h_threshold, joystick_v_threshold;
extern int joystick_shift_threshold,
  joystick_up_threshold,
  joystick_right_threshold,
  joystick_down_threshold,
  joystick_left_threshold,
  joystick_enter_threshold;
extern int joystick_h_stick,
  joystick_h_axis,
  joystick_v_stick,
  joystick_v_axis,
  joystick_up_button,
  joystick_right_button,
  joystick_down_button,
  joystick_left_button,
  joystick_enter_button,
  joystick_shift_button,
  joystick_time_button,
  joystick_pause_button;

extern double gamepad_rumble_gain;

/* functions */
void init_gamepad (void);
void finalize_gamepad (void);
int get_joystick_button (int n);
int get_joystick_axis (int stick, int axis);
int get_joystick_h_axis (void);
int get_joystick_v_axis (void);
ALLEGRO_JOYSTICK *calibrate_joystick (void);
void free_joystick (void);
ALLEGRO_EVENT_SOURCE *get_keyboard_event_source (void);
ALLEGRO_EVENT_SOURCE *get_joystick_event_source (void);
struct gamepad_state *get_gamepad_state (struct gamepad_state *k);
error_t joystick_info (void);
bool is_mod_key (int keycode);
int key2mod (int key);
char key2char (struct key *key);
int get_key_modifiers (void);
bool was_key_pressed (int modifiers, int keycode);
bool was_any_key_pressed (void);
bool was_char_pressed (char c);
bool was_joystick_button_pressed (int button);
void gamepad_rumble (double intensity, double duration);
void request_gamepad_rumble (double intensity, double duration);
void execute_haptic (void);
void reset_haptic (void);

#endif	/* MININIM_GAMEPAD_H */
