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
static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __tostring (lua_State *L);

static int play (lua_State *L);

void
define_L_mininim_audio_source (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_AUDIO_SOURCE);

  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_rawset (L, -3);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

void
L_pushaudiosource (lua_State *L, struct audio_source *as)
{
  struct audio_source *as_new = lua_newuserdata (L, sizeof (*as_new));
  luaL_getmetatable (L, L_MININIM_AUDIO_SOURCE);
  lua_setmetatable (L, -2);
  *as_new = *as;
}

int
L_mininim_audio_source (lua_State *L)
{
  const char *filename = luaL_checkstring (L, 1);
  const char *type_str = luaL_checkstring (L, 2);

  enum audio_type type = AUDIO_SAMPLE;
  if (! strcasecmp (type_str, "SAMPLE")) type = AUDIO_SAMPLE;
  else if (! strcasecmp (type_str, "STREAM")) type = AUDIO_STREAM;
  else return luaL_argerror (L, 2, "must be either \"SAMPLE\" or \"STREAM\"");

  struct audio_source as;
  load_audio (&as, type, filename);
  L_pushaudiosource (L, &as);
  return 1;
}

int
__gc (lua_State *L)
{
  struct audio_source *as = luaL_checkudata (L, 1, L_MININIM_AUDIO_SOURCE);
  if (as) destroy_audio (as);
  return 0;
}

int
__eq (lua_State *L)
{
  struct audio_source *as0 = luaL_checkudata (L, 1, L_MININIM_AUDIO_SOURCE);
  struct audio_source *as1 = luaL_checkudata (L, 2, L_MININIM_AUDIO_SOURCE);
  if (as0 && as1) lua_pushboolean (L, audio_source_eq (as0, as1));
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
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
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}

int
__tostring (lua_State *L)
{
  struct audio_source *as = luaL_checkudata (L, 1, L_MININIM_AUDIO_SOURCE);

  if (as)
    switch (as->type) {
    case AUDIO_SAMPLE:
      {
        char *str = xasprintf ("MININIM AUDIO SAMPLE %p", as->data.sample);
        lua_pushstring (L, str);
        al_free (str);
      }
      break;
    case AUDIO_STREAM:
      lua_pushfstring (L, "MININIM AUDIO STREAM %s", as->data.stream);
      break;
    default: assert (false); break;
    }
  else lua_pushstring (L, "MININIM AUDIO");

  return 1;
}

int
play (lua_State *L)
{
  struct audio_source *as =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_AUDIO_SOURCE);
  if (! as) return 0;
  int *id = NULL;
  struct pos *p = NULL;
  id = luaL_checkudata (L, 1, L_MININIM_ACTOR);
  p = luaL_checkudata (L, 1, L_MININIM_LEVEL_POSITION);
  play_audio (as, p, (id && *id < anima_nmemb) ? *id : -1);
  return 0;
}
