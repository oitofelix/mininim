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

bool left_key, right_key, shift_key, esc_key, pause_key, enter_key;

void
init_keyboard (void)
{
  if (! al_install_keyboard ())
    error (-1, 0, "%s (void): cannot install keyboard", __func__);
}

void
get_keyboard_state (void)
{
  al_get_keyboard_state (&keyboard_state);
  left_key = al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT);
  right_key = al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT);
  shift_key = al_key_down (&keyboard_state, ALLEGRO_KEY_LSHIFT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_RSHIFT);
  esc_key = al_key_down (&keyboard_state, ALLEGRO_KEY_ESCAPE);
  pause_key = al_key_down (&keyboard_state, ALLEGRO_KEY_PAUSE);
  enter_key = al_key_down (&keyboard_state, ALLEGRO_KEY_ENTER);
}

void
finalize_keyboard (void)
{
  al_uninstall_keyboard ();
}
