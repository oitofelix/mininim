/*
  L_mininim.actor.c -- mininim.actor script module;

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
static int new (lua_State *L);

void
define_L_mininim_actor (lua_State *L)
{
  luaL_newmetatable(L, "mininim.actor");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.actor");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pop (L, 1);

  /* mininim.actor[?] */
  define_L_mininim_actor_N (L);
}


int
__index (lua_State *L)
{
  const char *key;
  int index;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TNUMBER:
    index = lua_tonumber (L, 2);
    if (index >= anima_nmemb)
      return luaL_error (L, "mininim.actor[%d] does not exist", index);
    int *ud = lua_newuserdata (L, sizeof (*ud));
    luaL_getmetatable (L, "mininim.actor[?]");
    lua_setmetatable (L, -2);
    *ud = index;
    return 1;
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "new")) {
      lua_pushcfunction (L, new);
      return 1;
    }
    else return L_error_invalid_key_string (L, key, "mininim.actor");
  default: return L_error_invalid_key_type (L, type, "mininim.actor");
  }
}

int
new (lua_State *L)
{
  return 0;
}
