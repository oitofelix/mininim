/*
  L_mininim.math.c -- mininim.math script module;

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

static DECLARE_LUA (_div);
static DECLARE_LUA (mod);

static DECLARE_LUA (rshift);
static DECLARE_LUA (lshift);

static DECLARE_LUA (uint32);
static DECLARE_LUA (uint16);
static DECLARE_LUA (uint8);

static DECLARE_LUA (band);
static DECLARE_LUA (bor);
static DECLARE_LUA (bxor);
static DECLARE_LUA (bnot);

void
define_L_mininim_math (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_MATH);

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
    if (! strcasecmp (key, "div")) {
      lua_pushcfunction (L, _div);
      return 1;
    } else if (! strcasecmp (key, "mod")) {
      lua_pushcfunction (L, mod);
      return 1;
    } else if (! strcasecmp (key, "rshift")) {
      lua_pushcfunction (L, rshift);
      return 1;
    } else if (! strcasecmp (key, "lshift")) {
      lua_pushcfunction (L, lshift);
      return 1;
    } else if (! strcasecmp (key, "uint32")) {
      lua_pushcfunction (L, uint32);
      return 1;
    } else if (! strcasecmp (key, "uint16")) {
      lua_pushcfunction (L, uint16);
      return 1;
    } else if (! strcasecmp (key, "uint8")) {
      lua_pushcfunction (L, uint8);
      return 1;
    } else if (! strcasecmp (key, "band")) {
      lua_pushcfunction (L, band);
      return 1;
    } else if (! strcasecmp (key, "bor")) {
      lua_pushcfunction (L, bor);
      return 1;
    } else if (! strcasecmp (key, "bxor")) {
      lua_pushcfunction (L, bxor);
      return 1;
    } else if (! strcasecmp (key, "bnot")) {
      lua_pushcfunction (L, bnot);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM MATH INTERFACE");
  return 1;
}
END_LUA

BEGIN_LUA (_div)
{
  int64_t a = lua_tonumber (L, 1);
  int64_t b = lua_tonumber (L, 2);
  luaL_argcheck (L, b != 0, 2, "division by 0");
  lua_pushnumber (L, a / b);
  return 1;
}
END_LUA

BEGIN_LUA (mod)
{
  int64_t a = lua_tonumber (L, 1);
  int64_t b = lua_tonumber (L, 2);
  luaL_argcheck (L, b != 0, 2, "division by 0");
  lua_pushnumber (L, a % b);
  return 1;
}
END_LUA

BEGIN_LUA (rshift)
{
  int64_t a = lua_tonumber (L, 1);
  int64_t b = lua_tonumber (L, 2);
  lua_pushnumber (L, a >> b);
  return 1;
}
END_LUA

BEGIN_LUA (lshift)
{
  int64_t a = lua_tonumber (L, 1);
  int64_t b = lua_tonumber (L, 2);
  lua_pushnumber (L, a << b);
  return 1;
}
END_LUA

BEGIN_LUA (uint32)
{
  uint32_t a = lua_tonumber (L, 1);
  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (uint16)
{
  uint16_t a = lua_tonumber (L, 1);
  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (uint8)
{
  uint8_t a = lua_tonumber (L, 1);
  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (band)
{
  int64_t a = ~0;
  int i, n = lua_gettop (L);

  for (i = 1; i <= n; i++) a &= (int64_t) lua_tonumber (L, i);

  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (bor)
{
  int64_t a = 0;
  int i, n = lua_gettop (L);

  for (i = 1; i <= n; i++) a |= (int64_t) lua_tonumber (L, i);

  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (bxor)
{
  int64_t a = 0;
  int i, n = lua_gettop (L);

  for (i = 1; i <= n; i++) a ^= (int64_t) lua_tonumber (L, i);

  lua_pushnumber (L, a);
  return 1;
}
END_LUA

BEGIN_LUA (bnot)
{
  int64_t a = lua_tonumber (L, 1);
  lua_pushnumber (L, ~ a);
  return 1;
}
END_LUA
