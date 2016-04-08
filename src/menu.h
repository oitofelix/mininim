/*
  menu.h -- menu module;

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

#ifndef MININIM_MENU_H
#define MININIM_MENU_H

/* variable */
extern bool active_menu;
extern int menu_help;

/* functions */
void reset_menu (void);
char menu_opt (struct menu_item *menu, char *prefix);
char menu_enum (struct menu_item *menu, char *prefix);
char menu_bool (struct menu_item *menu, char *prefix, bool exclusive, ...);
char menu_int (int *v, int *b, int min, int max, char *pref_int, char *pref_bool);
char menu_list (int *dir0, int *dir1, int index, int min, int max, char *prefix);
bool was_menu_key_pressed (void);
bool was_menu_return_pressed (bool consume);

#endif	/* MININIM_MENU_H */
