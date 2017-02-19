/*
  L_mininim.setting.h -- mininim.setting script module;

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

#ifndef MININIM_L_MININIM_SETTING_H
#define MININIM_L_MININIM_SETTING_H

#define L_MININIM_SETTING L_MININIM ".setting"

void define_L_mininim_setting (lua_State *L);
void set_string_var (char **var, const char *value);
void L_set_string_var (lua_State *L, int index, char **var);

#endif	/* MININIM_L_MININIM_SETTING_H */
