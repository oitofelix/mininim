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

static int help;
static uint64_t help_cycles;
static size_t help_item;

char
process_menu (struct menu_item *menu, char *prefix)
{
  size_t i = 0;

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
    else if (help == 2 && key.keyboard.unichar == ESCAPE_KEY) help = 0;
    else if (key.keyboard.unichar == '?') help = 1;
    else if (key.keyboard.unichar == ESCAPE_KEY) {
      memset (&key, 0, sizeof (key));
      help = 0;
      return ESCAPE_KEY;
    } else {
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

  return 0;
}
