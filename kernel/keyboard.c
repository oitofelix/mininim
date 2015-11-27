/*
  keyboard.c -- keyboard module;

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

#include <error.h>
#include <allegro5/allegro.h>

/* keyboard state */
ALLEGRO_KEYBOARD_STATE keyboard_state;

int key; /* last key pressed */

/* real time keys */
bool up_key, down_key, left_key, right_key, shift_key, ctrl_key, esc_key,
  pause_key, enter_key, a_key, w_key, d_key, s_key, h_key, j_key,
  u_key, n_key, c_key;

void
init_keyboard (void)
{
  if (! al_install_keyboard ())
    error (-1, 0, "%s (void): cannot install keyboard", __func__);
}

void
finalize_keyboard (void)
{
  al_uninstall_keyboard ();
}

ALLEGRO_EVENT_SOURCE *
get_keyboard_event_source (void)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_keyboard_event_source ();
  if (! event_source)
    error (-1, 0, "%s: failed to get keyboard event source", __func__);
  return event_source;
}

void
get_keyboard_state (void)
{
  al_get_keyboard_state (&keyboard_state);
  up_key = al_key_down (&keyboard_state, ALLEGRO_KEY_UP);
  down_key = al_key_down (&keyboard_state, ALLEGRO_KEY_DOWN);
  left_key = al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT);
  right_key = al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT);
  shift_key = al_key_down (&keyboard_state, ALLEGRO_KEY_LSHIFT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_RSHIFT);
  ctrl_key = al_key_down (&keyboard_state, ALLEGRO_KEY_LCTRL)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_RCTRL);
  esc_key = al_key_down (&keyboard_state, ALLEGRO_KEY_ESCAPE);
  pause_key = al_key_down (&keyboard_state, ALLEGRO_KEY_PAUSE);
  enter_key = al_key_down (&keyboard_state, ALLEGRO_KEY_ENTER);
  a_key = al_key_down (&keyboard_state, ALLEGRO_KEY_A);
  w_key = al_key_down (&keyboard_state, ALLEGRO_KEY_W);
  d_key = al_key_down (&keyboard_state, ALLEGRO_KEY_D);
  s_key = al_key_down (&keyboard_state, ALLEGRO_KEY_S);
  h_key = al_key_down (&keyboard_state, ALLEGRO_KEY_H);
  j_key = al_key_down (&keyboard_state, ALLEGRO_KEY_J);
  u_key = al_key_down (&keyboard_state, ALLEGRO_KEY_U);
  n_key = al_key_down (&keyboard_state, ALLEGRO_KEY_N);
  c_key = al_key_down (&keyboard_state, ALLEGRO_KEY_C);
}

bool
was_key_pressed (int dkey, bool consume)
{
  if (key == dkey) {
    if (consume) key = 0;
    return true;
  } else return false;
}
