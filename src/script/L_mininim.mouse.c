/*
  L_mininim.mouse.c -- mininim.mouse script module;

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

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_mouse (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_MOUSE);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

BEGIN_LUA (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "position")) {
      L_pushposition (L, &mouse_pos);
      return 1;
    } else if (! strcasecmp (key, "selection_position")) {
      L_pushposition (L, &selection_pos);
      return 1;
    } else if (! strcasecmp (key, "selection_locked")) {
      lua_pushboolean (L, selection_locked);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  /* const char *key; */
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    /* key = lua_tostring (L, 2); */
    break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM MOUSE INTERFACE");
  return 1;
}
END_LUA
