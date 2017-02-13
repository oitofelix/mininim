/*
  L_mininim.level_con.c -- mininim.level[{?, ?, ?}] script module;

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

static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int activate (lua_State *L);

void
define_L_mininim_level_con (lua_State *L)
{
  luaL_newmetatable(L, "mininim.level[{?, ?, ?}]");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.level[{?, ?, ?}]");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

int
__index (lua_State *L)
{
  /* struct pos *p = L_check_type (L, 1, "mininim.level[{?, ?, ?}]"); */

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "activate")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, activate, 1);
      return 1;
    } else return L_error_invalid_key_string (L, key, "mininim.level[{?, ?, ?}]");
  default: return L_error_invalid_key_type (L, type, "mininim.level[{?, ?, ?}]");
  }
}

int
__newindex (lua_State *L)
{
  /* struct pos *p = L_check_type (L, 1, "mininim.level[{?, ?, ?}]"); */

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    return L_error_invalid_key_string (L, key, "mininim.level[{?, ?, ?}]");
  default: return L_error_invalid_key_type (L, type, "mininim.level[{?, ?, ?}]");
  }
}

int
activate (lua_State *L)
{
  struct pos *p = lua_touserdata (L, lua_upvalueindex (1));
  activate_con (p);
  return 0;
}
