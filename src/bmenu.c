/*
  bmenu.c -- bottom menu module;

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

#include "mininim.h"

bool active_menu;
int bmenu_help;
static uint64_t help_cycles;
static size_t help_item;

void
reset_menu (void)
{
  bmenu_help = 0;
  active_menu = false;
}

char
bmenu_opt (struct bmenu_item *menu, char *prefix)
{
  size_t i = 0;
  char c;

  active_menu = true;

  if (! bmenu_help) editor_msg (prefix, EDITOR_CYCLES_0);

  if (was_any_key_pressed ()
      && key.keycode != ALLEGRO_KEY_BACKSPACE
      && key2char (&key) != '/') {
    if (bmenu_help == 1) {
      while (menu[i].key) {
        if (was_char_pressed (menu[i].key)) {
          bmenu_help = 2;
          help_cycles = 18;
          help_item = i;
          break;
        }
        i++;
      }
    } else if (was_char_pressed ('?')) bmenu_help = 1;
    else {
      while (menu[i].key) {
        if (was_char_pressed (menu[i].key)) {
          bmenu_help = 0;
          return menu[i].key;
        }
        i++;
      }
    }
    if (! menu[i].key && ! was_char_pressed ('?'))
      bmenu_help = 0;
  }

  if (bmenu_help == 1)
    editor_msg ("PRESS KEY FOR HELP", EDITOR_CYCLES_0);
  if (bmenu_help == 2 && --help_cycles)
    editor_msg (menu[help_item].desc, EDITOR_CYCLES_0);
  else if (bmenu_help == 2) bmenu_help = 0;

  c = 0;

  if (was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)) {
    if (bmenu_help == 0) c = 1;
    else bmenu_help = 0;
  }

  if (was_char_pressed ('/')) {
    if (bmenu_help == 0) c = -1;
    else bmenu_help = 0;
  }

  return c;
}

char
bmenu_enum (struct bmenu_item *menu, char *prefix)
{
  int i = 0;
  char *bmenu_str = NULL, *tmp_str = NULL;
  while (menu[i].key) {
    if (bmenu_str) {
      tmp_str = bmenu_str;
      bmenu_str = xasprintf ("%s%c", bmenu_str, menu[i].key);
    } else {
      bmenu_str = xasprintf ("%s%c", prefix ? prefix : "", menu[i].key);
    }
    al_free (tmp_str);
    i++;
  }
  if (bmenu_str) {
    tmp_str = bmenu_str;
    bmenu_str = xasprintf ("%s%c", bmenu_str, '?');
  }
  al_free (tmp_str);
  char c = bmenu_opt (menu, bmenu_str);
  al_free (bmenu_str);
  return c;
}

char
bmenu_bool (struct bmenu_item *menu, char *prefix, bool exclusive, ...)
{
  va_list ap;
  va_start (ap, exclusive);

  int i = 0;
  char *bmenu_str = NULL, *tmp_str = NULL;
  while (menu[i].key) {
    bool *b = va_arg (ap, bool *);
    if (bmenu_str) {
      tmp_str = bmenu_str;
      bmenu_str = xasprintf ("%s%c%c", bmenu_str,
                 menu[i].key, *b ? '*' : '-');
    } else {
      bmenu_str = xasprintf ("%s%c%c", prefix ? prefix : "",
                 menu[i].key, *b ? '*' : '-');
    }
    al_free (tmp_str);
    i++;
  }
  if (bmenu_str) {
    tmp_str = bmenu_str;
    bmenu_str = xasprintf ("%s%c", bmenu_str, '?');
  }
  va_end (ap);

  al_free (tmp_str);
  char c = bmenu_opt (menu, bmenu_str);

  if (c > 1) {
    i = 0;
    va_start (ap, exclusive);
    while (menu[i].key) {
      bool *b = va_arg (ap, bool *);

      if (exclusive) {
        if (menu[i].key == c)
          *b = true;
        else *b = false;
      } else if (menu[i].key == c) {
        *b = ! *b;
        break;
      }

      i++;
    }
    va_end (ap);
  }

  al_free (bmenu_str);
  return c;
}

char
bmenu_int (int *v, int *b, int min, int max, char *pref_int, char *pref_bool)
{
  active_menu = true;

  char *str;

  if (*v < min) *v = min;
  if (*v > max) *v = max;

  char *b_str, *min_str, *max_str;

  if (b && pref_bool) b_str = xasprintf (" %s", *b ? pref_bool : "-");
  else b_str = xasprintf ("%s", "");

  if (min == INT_MIN) min_str = xasprintf ("%s", "<");
  else min_str = xasprintf ("%i", min);

  if (max == INT_MAX) max_str = xasprintf ("%s", ">");
  else max_str = xasprintf ("%i", max);

  str = xasprintf ("%s %s-%s:%i%s",
             pref_int, min_str, max_str, *v, b_str);

  editor_msg (str, EDITOR_CYCLES_0);

  al_free (b_str);
  al_free (min_str);
  al_free (max_str);
  al_free (str);

  if (! was_any_key_pressed ()) return 0;

  int c = toupper (key2char (&key));

  if (was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)) return 1;

  int r;
  switch (c) {
  case '-': case '_':
    *v = int_to_range (*v - 1, min, max);
    break;
  case '+': case '=':
    *v = int_to_range (*v + 1, min, max);
    break;
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9':
    str = xasprintf ("%i%c", *v, c);
    if (sscanf (str, "%d", &r) != 1) r = *v;
    al_free (str);
    *v = int_to_range (r, min, max);
    break;
  case '\\':
    *v = int_to_range (*v / 10, min, max);
    break;
  case ' ':
    if (b) *b = ! *b;
    break;
  case '/': return -1;
  case 'N':
    *v = int_to_range (-*v, min, max);
    break;
  default: c = 0; break;
  }

  return c;
}

char
bmenu_list (int *dir0, int *dir1, int index, int min, int max, char *prefix)
{
  active_menu = true;

  char *str;

  if (index >= min && index <= max)
    str = xasprintf ("%s %i %s%s", prefix, index,
               dir0 ? "-+" : "", dir1 ? "<>" : "");
  else
    str = xasprintf ("%s NONE %s%s", prefix,
               dir0 ? "-+" : "", dir1 ? "<>" : "");

  editor_msg (str, EDITOR_CYCLES_0);
  al_free (str);

  if (! was_any_key_pressed ()) return 0;

  int c = toupper (key2char (&key));

  if (was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)) return 1;

  switch (c) {
  case '-': case '_':
    if (dir0) *dir0 = -1;
    break;
  case '+': case '=':
    if (dir0) *dir0 = +1;
    break;
  case ',': case '<':
    if (dir1) *dir1 = -1;
    break;
  case '.': case '>':
    if (dir1) *dir1 = +1;
    break;
  case '/':
    return -1;
  default:
    if (dir0) *dir0 = 0;
    if (dir1) *dir1 = 0;
    c = 0;
    break;
  }

  return c;
}

bool
was_bmenu_key_pressed (void)
{
  if (! was_any_key_pressed ()) return false;

  switch (toupper (key2char (&key))) {
  case '0': case '1': case '2': case '3': case '4': case '5':
  case '6': case '7': case '8': case '9': case 'A': case 'B':
  case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
  case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
  case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '?': case '-': case '_': case '+': case '=': case '\\':
  case ' ': case '/': case ',': case '.': case '<': case '>':
  case '#': case '!':
    return true;
  }

  if (was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)) return true;

  return false;
}

bool
was_bmenu_return_pressed (bool consume)
{
  return was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)
    || was_char_pressed ('/');
}
