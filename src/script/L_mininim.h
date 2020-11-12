/*
  L_mininim.h -- mininim script module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_L_MININIM_H
#define MININIM_L_MININIM_H

#define L_MININIM "MININIM"

int push_lua_table (lua_State *L);
int push_lua_table_field (lua_State *L, const char *k);
void define_L_mininim (lua_State *L);
void error_lua_invalid (const char *template, ...)
  __attribute__ ((format (gnu_printf, 1, 2)));
bool call_lua_function (lua_State *L, const char *name, int nargs,
                        int nresults);
bool run_lua_hook (lua_State *L, const char *name);

extern int lua_table_ref;

#endif	/* MININIM_L_MININIM_H */
