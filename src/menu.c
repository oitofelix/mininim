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
  char c;

  active_menu = true;

  if (key.keyboard.unichar > 0
      && key.keyboard.keycode != ALLEGRO_KEY_BACKSPACE) {
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
    else if (key.keyboard.unichar == '?') help = 1;
    else {
      while (menu[i].key) {
        if (menu[i].key == toupper (key.keyboard.unichar)) {
          help = 0;
          memset (&key, 0, sizeof (key));
          return menu[i].key;
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

  if (key.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
    if (help == 0) c = BACKSPACE_KEY;
    memset (&key, 0, sizeof (key));
    help = 0;
  }

  return c;
}

int
menu_int (int *v, int *b, int min, int max, char *pref_int, char *pref_bool)
{
  active_menu = true;

  char *str;

  if (*b < 0)
    xasprintf (&str, "%s %i-%i:%i", pref_int, min, max, *v);
  else
    xasprintf (&str, "%s %i-%i:%i %s",
               pref_int, min, max, *v, *b ? pref_bool : "-");
  draw_bottom_text (NULL, str);
  al_free (str);

  int c = toupper (key.keyboard.unichar);
  int keycode = key.keyboard.keycode;

  memset (&key, 0, sizeof (key));

  if (keycode == ALLEGRO_KEY_BACKSPACE) return 1;

  int r;
  switch (c) {
  case '-': case '_':
    r = *v - 1;
    *v = r >= min ? r : *v;
    break;
  case '+': case '=':
    r = *v + 1;
    *v = r <= max ? r : *v;
    break;
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9':
    xasprintf (&str, "%i%c", *v, c);
    sscanf (str, "%d", &r);
    al_free (str);
    *v = r <= max ? r : *v;
    break;
  case '\\':
    r = *v / 10;
    *v = r >= min ? r : *v;
    break;
  case ' ':
    if (*b >= 0) *b = ! *b;
    break;
  case '/':
    return -1;
  }

  return 0;
}

int
menu_list (int *dir0, int *dir1, int index, char *prefix)
{
  active_menu = true;

  char *str;

  xasprintf (&str, "%s %i +-<>", prefix, index);
  draw_bottom_text (NULL, str);
  al_free (str);

  int c = toupper (key.keyboard.unichar);
  int keycode = key.keyboard.keycode;

  memset (&key, 0, sizeof (key));

  if (keycode == ALLEGRO_KEY_BACKSPACE) return 1;

  switch (c) {
  case '-': case '_':
    *dir0 = -1;
    break;
  case '+': case '=':
    *dir0 = +1;
    break;
  case ',': case '<':
    *dir1 = -1;
    break;
  case '.': case '>':
    *dir1 = +1;
    break;
  case '/':
    return -1;
  default:
    *dir0 = 0;
    *dir1 = 0;
    break;
  }

  return 0;
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
  case '?': case '-': case '_': case '+': case '=': case '\\':
  case ' ': case '/': case ',': case '.': case '<': case '>':
    return true;
  }

  if (key.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) return true;

  return false;
}
