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

#ifndef MININIM_KEYBOARD_H
#define MININIM_KEYBOARD_H

#define IGNORED_KEYBOARD_MODIFIERS \
  (ALLEGRO_KEYMOD_SCROLLLOCK | ALLEGRO_KEYMOD_NUMLOCK \
   | ALLEGRO_KEYMOD_CAPSLOCK)

struct keyboard_state {
  bool up, down, left, right, shift, enter;
};

/* variables */
extern ALLEGRO_KEYBOARD_STATE keyboard_state;
extern ALLEGRO_EVENT key;

/* functions */
void init_keyboard (void);
void finalize_keyboard (void);
ALLEGRO_EVENT_SOURCE *get_keyboard_event_source (void);
struct keyboard_state *get_keyboard_state (struct keyboard_state *k);
bool was_key_pressed (int keycode, int unichar, unsigned int modifiers,
                      bool consume);

#endif	/* MININIM_KEYBOARD_H */
