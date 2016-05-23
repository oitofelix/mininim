/*
  gamepad.c -- gamepad module;

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
bool flip_gamepad_horizontal, flip_gamepad_vertical;

/* keyboard state */
ALLEGRO_KEYBOARD_STATE keyboard_state;

ALLEGRO_EVENT key; /* last key pressed */
int button = -1;

ALLEGRO_JOYSTICK *joystick;
ALLEGRO_JOYSTICK_STATE joystick_state;
static float joystick_h_center, joystick_v_center,
  joystick_shift_released,
  joystick_up_released,
  joystick_right_released,
  joystick_down_released,
  joystick_left_released,
  joystick_enter_released;
float joystick_h_threshold = 0.1,
  joystick_v_threshold = 0.8;
int joystick_shift_threshold = 100,
  joystick_up_threshold = 100,
  joystick_right_threshold = 100,
  joystick_down_threshold = 100,
  joystick_left_threshold = 100,
  joystick_enter_threshold = 100;
int joystick_h_stick = 0,
  joystick_h_axis = 0,
  joystick_v_stick = 0,
  joystick_v_axis = 1,
  joystick_up_button = 0,
  joystick_right_button = 1,
  joystick_down_button = 2,
  joystick_left_button = 3,
  joystick_enter_button = 4,
  joystick_shift_button = 5,
  joystick_time_button = 8,
  joystick_pause_button = 9;

void
init_gamepad (void)
{
  if (! al_install_keyboard ())
    error (-1, 0, "%s (void): cannot install keyboard", __func__);

  if (! al_install_joystick ())
    error (-1, 0, "%s (void): cannot install joystick", __func__);

  calibrate_joystick ();
}

void
finalize_gamepad (void)
{
  al_uninstall_keyboard ();
  al_uninstall_joystick ();
}

int
get_joystick_button (int n)
{
  if (! joystick) return 0;
  int num_buttons = al_get_joystick_num_buttons (joystick);
  if (n >= num_buttons) return 0;
  al_get_joystick_state (joystick, &joystick_state);
  return joystick_state.button[n];
}

int
get_joystick_axis (int stick, int axis)
{
  if (! joystick) return 0;
  int num_sticks = al_get_joystick_num_sticks (joystick);
  if (stick >= num_sticks) return 0;
  int num_axes = al_get_joystick_num_axes (joystick, stick);
  if (axis >= num_axes) return 0;
  al_get_joystick_state (joystick, &joystick_state);
  return joystick_state.stick[stick].axis[axis];
}

int
get_joystick_h_axis (void)
{
  return get_joystick_axis (joystick_h_stick, joystick_h_axis);
}

int
get_joystick_v_axis (void)
{
  return get_joystick_axis (joystick_v_stick, joystick_v_axis);
}

void
calibrate_joystick (void)
{
  al_reconfigure_joysticks();
  joystick = al_get_joystick (0);
  if (joystick) {
    joystick_h_center = get_joystick_h_axis ();
    joystick_v_center = get_joystick_v_axis ();
    joystick_shift_released = get_joystick_button (joystick_shift_button);
    joystick_up_released = get_joystick_button(joystick_up_button);
    joystick_right_released = get_joystick_button(joystick_right_button);
    joystick_down_released = get_joystick_button(joystick_down_button);
    joystick_left_released = get_joystick_button(joystick_left_button);
    joystick_enter_released = get_joystick_button(joystick_enter_button);
  }
}

ALLEGRO_EVENT_SOURCE *
get_keyboard_event_source (void)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_keyboard_event_source ();
  if (! event_source)
    error (-1, 0, "%s: failed to get keyboard event source", __func__);
  return event_source;
}

ALLEGRO_EVENT_SOURCE *
get_joystick_event_source (void)
{
  ALLEGRO_EVENT_SOURCE *event_source = al_get_joystick_event_source ();
  if (! event_source)
    error (-1, 0, "%s: failed to get joystick event source", __func__);
  return event_source;
}

struct gamepad_state *
get_gamepad_state (struct gamepad_state *k)
{
  al_get_keyboard_state (&keyboard_state);
  bool up, down, left, right;

  up = al_key_down (&keyboard_state, ALLEGRO_KEY_UP)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_7)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_8)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_9)
    || (get_joystick_v_axis () <
        joystick_v_center - joystick_v_threshold)
    || (get_joystick_button (joystick_up_button) >
        joystick_up_released + joystick_up_threshold);
  down = al_key_down (&keyboard_state, ALLEGRO_KEY_DOWN)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_5)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_1)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_2)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_3)
    || (get_joystick_v_axis () >
        joystick_v_center + joystick_v_threshold)
    || (get_joystick_button (joystick_down_button) >
        joystick_down_released + joystick_down_threshold);
  left = al_key_down (&keyboard_state, ALLEGRO_KEY_LEFT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_1)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_7)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_4)
    || (get_joystick_h_axis () <
        joystick_h_center - joystick_h_threshold)
    || (get_joystick_button (joystick_left_button) >
        joystick_left_released + joystick_left_threshold);
  right = al_key_down (&keyboard_state, ALLEGRO_KEY_RIGHT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_3)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_6)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_9)
    || (get_joystick_h_axis () >
        joystick_h_center + joystick_h_threshold)
    || (get_joystick_button (joystick_right_button) >
        joystick_right_released + joystick_right_threshold);

  k->up = up;
  k->down = down;
  k->left = left;
  k->right = right;

  if (flip_gamepad_vertical)
    k->up = down, k->down = up;

  if (flip_gamepad_horizontal)
    k->left = right, k->right = left;

  k->shift = al_key_down (&keyboard_state, ALLEGRO_KEY_LSHIFT)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_RSHIFT)
    || (get_joystick_button (joystick_shift_button) >
        joystick_shift_released + joystick_shift_threshold);
  k->enter = al_key_down (&keyboard_state, ALLEGRO_KEY_ENTER)
    || al_key_down (&keyboard_state, ALLEGRO_KEY_PAD_ENTER)
    || (get_joystick_button (joystick_enter_button) >
        joystick_enter_released + joystick_enter_threshold);

  return k;
}

error_t
joystick_info (void)
{
  int i, j;

  if (! al_is_joystick_installed ()) {
    init_gamepad ();
    calibrate_joystick ();
  }
  if (! joystick) return ENXIO;

  al_rest (0.1);

  al_get_joystick_state (joystick, &joystick_state);

  printf ("Name: \"%s\"\n", al_get_joystick_name (joystick));
  printf ("Sticks: %i\n", al_get_joystick_num_sticks (joystick));

  for (i = 0; i < al_get_joystick_num_sticks (joystick); i++) {
    printf ("  %i: %s [%i axes]\n", i, al_get_joystick_stick_name (joystick, i),
            al_get_joystick_num_axes (joystick, i));
    for (j = 0; j < al_get_joystick_num_axes (joystick, i); j++)
      printf ("    %i: %s (%f)", j, al_get_joystick_axis_name (joystick, i, j),
              joystick_state.stick[i].axis[j]);
    printf ("\n");
  }

  printf ("Buttons: %i\n", al_get_joystick_num_buttons (joystick));

  for (i = 0; i < al_get_joystick_num_buttons (joystick); i++) {
    printf ("  %i: %s (%i)\n", i, al_get_joystick_button_name (joystick, i),
            joystick_state.button[i]);
  }

  return 0;
}

bool
was_key_pressed (int keycode, int unichar, unsigned int modifiers,
                 bool consume)
{
  key.keyboard.modifiers &= ~ IGNORED_KEYBOARD_MODIFIERS;

  if ((keycode == key.keyboard.keycode && unichar <= 0
       && modifiers == key.keyboard.modifiers)
      || (unichar == key.keyboard.unichar && unichar > 0
          && (modifiers == key.keyboard.modifiers
              || modifiers == -1))) {
    if (consume) memset (&key, 0, sizeof (key));
    return true;
  } else return false;
}

bool
was_button_pressed (int _button, bool consume)
{
  if (button == _button) {
    if (consume) button = -1;
    return true;
  } else return false;
}
