/*
  L_mininim.setting.c -- mininim.setting script module;

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

void
define_L_mininim_setting (lua_State *L)
{
  luaL_newmetatable(L, "mininim.setting");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.setting");
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
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "audio_mode")) {
      lua_pushstring (L, audio_mode);
      return 1;
    } else if (! strcasecmp (key, "video_mode")) {
      lua_pushstring (L, video_mode);
      return 1;
    } else return L_error_invalid_key_string (L, key, "mininim.setting");
  default: return L_error_invalid_key_type (L, type, "mininim.setting");
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
    if (! strcasecmp (key, "audio_mode")) {
      L_set_audio_mode (L, 3);
      return 0;
    } else if (! strcasecmp (key, "video_mode")) {
      L_set_video_mode (L, 3);
      return 0;
    } else return L_error_invalid_key_string (L, key, "mininim.setting");
  default: return L_error_invalid_key_type (L, type, "mininim.setting");
  }
}

void
L_set_audio_mode (lua_State *L, int index)
{
  const char *value = lua_tostring (L, 3);
  if (value) {
    if (audio_mode) al_free (audio_mode);
    audio_mode = xasprintf ("%s", value);
  } else luaL_error
           (L, "mininim.setting.audio_mode must be a string");
}

void
L_set_video_mode (lua_State *L, int index)
{
  const char *value = lua_tostring (L, 3);
  if (value) {
    if (video_mode) al_free (video_mode);
    video_mode = xasprintf ("%s", value);
  } else luaL_error
           (L, "mininim.setting.video_mode must be a string");
}
