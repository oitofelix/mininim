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

#include "mininim.h"

/* flip settings */
bool flip_keyboard_horizontal, flip_keyboard_vertical;

/* keyboard state */
ALLEGRO_KEYBOARD_STATE keyboard_state;

ALLEGRO_EVENT key; /* last key pressed */

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

struct keyboard_state *
get_keyboard_state (struct keyboard_state *k)
{
  al_get_keyboard_state (&keyboard_state);
  bool up, down, left, right;

  up = al_key_down (&keyboard_state, ALLEGRO_KEY_UP);
  down = al_key_down (&keyboard_state, ALLEGRO_KEY_DOWN);
  left = al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT);
  right = al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT);

  k->up = up;
  k->down = down;
  k->left = left;
  k->right = right;

  if (flip_keyboard_vertical)
    k->up = down, k->down = up;

  if (flip_keyboard_horizontal)
    k->left = right, k->right = left;

  k->shift = al_key_down (&keyboard_state, ALLEGRO_KEY_LSHIFT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_RSHIFT);
  k->enter = al_key_down (&keyboard_state, ALLEGRO_KEY_ENTER);

  return k;
}

bool
was_key_pressed (int keycode, int unichar, unsigned int modifiers,
                 bool consume)
{
  key.keyboard.modifiers &= ~ IGNORED_KEYBOARD_MODIFIERS;

  if ((((keycode == key.keyboard.keycode && unichar <= 0)
        || (unichar == key.keyboard.unichar && unichar > 0))
       && modifiers == key.keyboard.modifiers)) {
    if (consume) memset (&key, 0, sizeof (key));
    return true;
  } else return false;
}
