/*
  menu.c -- menu module;

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

bool active_menu;
static int help;
static uint64_t help_cycles;
static size_t help_item;

void
reset_menu (void)
{
  help = 0;
  active_menu = false;
}

char
process_menu (struct menu_item *menu, char *prefix)
{
  size_t i = 0;
  active_menu = false;
  char c;

  if (key.keyboard.unichar > 0) {
    if (help == 1) {
      while (menu[i].key) {
        if (menu[i].key == toupper (key.keyboard.unichar)) {
          help = 2;
          help_cycles = 18;
          help_item = i;
          break;
        }
        i++;
      }
    }
    else if (help == 2 && key.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) help = 0;
    else if (key.keyboard.unichar == '?') help = 1;
    else if (key.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
      memset (&key, 0, sizeof (key));
      help = 0;
      c = BACKSPACE_KEY;
      goto end;
    } else {
      while (menu[i].key) {
        if (menu[i].key == toupper (key.keyboard.unichar)) {
          help = 0;
          memset (&key, 0, sizeof (key));
          c = menu[i].key;
          goto end;
        }
        i++;
      }
    }
    if (! menu[i].key && key.keyboard.unichar != '?') help = 0;
    memset (&key, 0, sizeof (key));
  }

  if (help == 1)
    draw_bottom_text (NULL, "PRESS KEY FOR HELP");
  if (help == 2 && --help_cycles)
    draw_bottom_text (NULL, menu[help_item].desc);
  else if (help == 2) help = 0;

  if (! help) {
    i = 0;
    char *menu_str = NULL, *tmp_str = NULL;
    while (menu[i].key) {
      if (menu_str) {
        tmp_str = menu_str;
        xasprintf (&menu_str, "%s%c", menu_str, menu[i].key);
      } else {
        xasprintf (&menu_str, "%s%c", prefix ? prefix : "", menu[i].key);
      }
      if (tmp_str) al_free (tmp_str);
      i++;
    }
    if (menu_str) {
      tmp_str = menu_str;
      xasprintf (&menu_str, "%s%c", menu_str, '?');
    }
    if (tmp_str) al_free (tmp_str);
    draw_bottom_text (NULL, menu_str);
    al_free (menu_str);
  }

  c = 0;

 end:
  active_menu = true;
  return c;
}

bool
was_menu_key_pressed (void)
{
  switch (toupper (key.keyboard.unichar)) {
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9': case 'A': case 'B':
  case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
  case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
  case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '?': case '-': case '+': case '=':
    return true;
  }

  if (key.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) return true;

  return false;
}
