/*
  L_mininim.multiroom.c -- mininim.multiroom script module;

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

#include "mininim.h"

static DEFUN (__eq);
static DEFUN (__index);
static DEFUN (__newindex);
static DEFUN (__tostring);

void
define_L_mininim_multiroom (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_MULTIROOM);

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

DEFUN (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}

DEFUN (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "width")) {
      lua_pushnumber (L, global_mr.w);
      return 1;
    } else if (! strcasecmp (key, "height")) {
      lua_pushnumber (L, global_mr.h);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "width")) {
      int w = lua_tonumber (L, 3);
      mr_set_dim (&global_mr, w, global_mr.h);
      return 0;
    } else if (! strcasecmp (key, "height")) {
      int h = lua_tonumber (L, 3);
      mr_set_dim (&global_mr, global_mr.w, h);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__tostring)
{
  lua_pushstring (L, "MININIM MULTIROOM INTERFACE");
  return 1;
}
