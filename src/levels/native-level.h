/*
  native-level.h -- native level module;

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

#ifndef MININIM_NATIVE_LEVEL_H
#define MININIM_NATIVE_LEVEL_H

/* functions */
void play_native_level (int number);
void next_native_level (int number);
void load_native_level (int number, struct level *l);
bool save_native_level (struct level *l, char *filename);
char *get_confg_str (struct pos *p);
char *get_conbg_str (struct pos *p);
char *get_conext_str (struct pos *p);

/* variables */
extern struct level native_level;

#endif	/* MININIM_NATIVE_LEVEL_H */
