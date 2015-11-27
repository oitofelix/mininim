/*
  keyboard.h -- keyboard module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef FREEPOP_KEYBOARD_H
#define FREEPOP_KEYBOARD_H

/* variables */
extern ALLEGRO_KEYBOARD_STATE keyboard_state;
extern int key;
extern bool up_key, down_key, left_key, right_key, shift_key, ctrl_key,
  esc_key, pause_key, enter_key, a_key, w_key, d_key, s_key,
  h_key, j_key, u_key, n_key, c_key;

/* functions */
void init_keyboard (void);
void finalize_keyboard (void);
ALLEGRO_EVENT_SOURCE *get_keyboard_event_source (void);
void get_keyboard_state (void);
bool was_key_pressed (int dkey, bool consume);

#endif	/* FREEPOP_KEYBOARD_H */
