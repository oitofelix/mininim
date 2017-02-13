/*
  L_mininim.audio_res.c -- mininim.audio[?][?] script module;

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

static int __gc (lua_State *L);
static int __index (lua_State *L);

static int play (lua_State *L);

void
define_L_mininim_audio_res (lua_State *L)
{
  luaL_newmetatable(L, "mininim.audio[?][?]");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.audio[?][?]");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

int
__gc (lua_State *L)
{
  struct audio_source *as = lua_touserdata (L, 1);
  destroy_audio (as);
  return 0;
}

int
__index (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "play")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, play, 1);
      return 1;
    } else L_error_invalid_key_string (L, key, "mininim.audio[?][?]");
  default: return L_error_invalid_key_type (L, type, "mininim.audio[?][?]");
  }
}

int
play (lua_State *L)
{
  struct audio_source *as = lua_touserdata (L, lua_upvalueindex (1));
  int *id = NULL;
  struct pos *p = NULL;
  id = luaL_checkudata (L, 1, "mininim.actor[?]");
  p = luaL_checkudata (L, 1, "mininim.level[{?, ?, ?}]");
  play_audio (as, p, (id && *id < anima_nmemb) ? *id : -1);
  return 0;
}
