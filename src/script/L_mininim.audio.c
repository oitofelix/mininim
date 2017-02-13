/*
  L_mininim.audio.c -- mininim.audio script module;

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

static const char audio_mode_key = 'k';

static int __index (lua_State *L);
static int __newindex (lua_State *L);

static int load (lua_State *L);

void
define_L_mininim_audio (lua_State *L)
{
  luaL_newmetatable(L, "mininim.audio");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.audio");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_newtable (L);
  L_set_registry (L, &audio_mode_key);

  lua_pop (L, 1);

  /* mininim.audio[?][?] */
  define_L_mininim_audio_res (L);
}

int
__index (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "load")) {
      lua_pushcfunction (L, load);
      return 1;
    } if (! strcasecmp (key, "current")) {
      if (audio_mode) {
        L_get_registry (L, &audio_mode_key);
        lua_pushstring (L, audio_mode);
        lua_gettable (L, 1);
      } else lua_pushnil (L);
      return 1;
    } else {
      L_get_registry (L, &audio_mode_key);
      lua_replace (L, 1);
      lua_gettable (L, 1);
      return 1;
    }
  default: return L_error_invalid_key_type (L, type, "mininim.audio");
  }
}

int
__newindex (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "load")) {
      return luaL_error (L, "mininim.level.load is read-only");
    } else if (! strcasecmp (key, "current")) {
      L_set_audio_mode (L, 3);
      return 0;
    } else {
      L_get_registry (L, &audio_mode_key);
      lua_replace (L, 1);
      lua_settable (L, 1);
      return 0;
    }
  default: return L_error_invalid_key_type (L, type, "mininim.audio");
  }
}

int
load (lua_State *L)
{
  const char *filename = luaL_checkstring (L, 1);
  const char *type_str = luaL_checkstring (L, 2);

  enum audio_type type = AUDIO_SAMPLE;
  if (! strcasecmp (type_str, "SAMPLE")) type = AUDIO_SAMPLE;
  else if (! strcasecmp (type_str, "STREAM")) type = AUDIO_STREAM;
  else return luaL_argerror (L, 2, "must be either \"SAMPLE\" or \"STREAM\"");

  struct audio_source *as = lua_newuserdata (L, sizeof (*as));
  load_audio (as, type, filename);
  luaL_getmetatable (L, "mininim.audio[?][?]");
  lua_setmetatable (L, -2);

  return 1;
}

void
L_play_audio (char *key, struct pos *p, int anim_id)
{
  L_get_registry (L, &audio_mode_key);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return;
  }

  lua_pushstring (L, audio_mode);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return;
  }

  lua_pushstring (L, key);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  struct audio_source *as = luaL_checkudata (L, -1, "mininim.audio[?][?]");

  lua_pop (L, 1);

  if (! as) return;

  play_audio (as, p, anim_id);
}
