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

static int audio_mode_ref = LUA_NOREF;

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_audio (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_AUDIO);

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

  lua_newtable (L);
  L_set_registry_by_ref (L, &audio_mode_ref);

  lua_pop (L, 1);

  /* mininim.audio.source */
  define_L_mininim_audio_source (L);
}

void
L_play_audio (lua_State *L, char *key, struct pos *p, int anim_id)
{
  L_get_registry_by_ref (L, audio_mode_ref);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return;
  }

  lua_pushstring (L, audio_mode);
  lua_rawget (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return;
  }

  lua_pushstring (L, key);
  lua_rawget (L, -2);
  lua_remove (L, -2);

  struct audio_source *as = luaL_checkudata (L, -1, L_MININIM_AUDIO_SOURCE);

  lua_pop (L, 1);

  if (! as) return;

  play_audio (as, p, anim_id);
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
    if (! strcasecmp (key, "source")) {
      lua_pushcfunction (L, L_mininim_audio_source);
      return 1;
    } else if (! strcasecmp (key, "current")) {
      assert (audio_mode);
      L_get_registry_by_ref (L, audio_mode_ref);
      lua_pushstring (L, audio_mode);
      lua_rawget (L, -2);
      return 1;
    } else {
      L_get_registry_by_ref (L, audio_mode_ref);
      lua_replace (L, 1);
      lua_rawget (L, 1);
      return 1;
    }
  default: break;
  }

  lua_pushnil (L);
  return 1;
}
END_LUA

BEGIN_LUA (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "current")) {
      L_set_string_var (L, 3, &audio_mode);
      return 0;
    } else {
      L_get_registry_by_ref (L, audio_mode_ref);
      lua_replace (L, 1);
      lua_settable (L, 1);
      return 0;
    }
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM AUDIO INTERFACE");
  return 1;
}
END_LUA
