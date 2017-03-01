/*
  L_mininim.video.c -- mininim.video script module;

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

static int video_mode_ref = LUA_NOREF;

static DECLARE_LUA ( __eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_video (lua_State *L)
{
  /* mininim.video */
  luaL_newmetatable(L, L_MININIM_VIDEO);

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

  /* video mode table */
  lua_newtable (L);
  L_set_registry_by_ref (L, &video_mode_ref);

  /* mininim.video.bitmap */
  define_L_mininim_video_bitmap (L);

  /* mininim.video.color */
  define_L_mininim_video_color (L);

  /* mininim.video.coordinate */
  define_L_mininim_video_coordinate (L);

  /* mininim.video.rectangle */
  define_L_mininim_video_rectangle (L);
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
    if (! strcasecmp (key, "bitmap")) {
      lua_pushcfunction (L, L_mininim_video_bitmap);
      return 1;
    } else if (! strcasecmp (key, "color")) {
      lua_pushcfunction (L, L_mininim_video_color);
      return 1;
    } else if (! strcasecmp (key, "coordinate")) {
      lua_pushcfunction (L, L_mininim_video_coordinate);
      return 1;
    } else if (! strcasecmp (key, "rectangle")) {
      lua_pushcfunction (L, L_mininim_video_rectangle);
      return 1;
    } else if (! strcasecmp (key, "env_mode")) {
      if (! strcasecmp (env_mode, "ORIGINAL"))
        lua_pushstring (L, env_mode_string (global_level.em));
      else lua_pushstring (L, env_mode);
      return 1;
    } else if (! strcasecmp (key, "hue_mode")) {
      if (! strcasecmp (hue_mode, "ORIGINAL"))
        lua_pushstring (L, hue_mode_string (global_level.hue));
      else lua_pushstring (L, hue_mode);
      return 1;
    } else if (! strcasecmp (key, "palette_cache_size_limit")) {
      lua_pushnumber (L, palette_cache_size_limit);
      return 1;
    } else if (! strcasecmp (key, "current")) {
      assert (video_mode);
      L_get_registry_by_ref (L, video_mode_ref);
      lua_pushstring (L, video_mode);
      lua_rawget (L, 1);
      return 1;
    } else {
      L_get_registry_by_ref (L, video_mode_ref);
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
      L_set_string_var (L, 3, &video_mode);
      return 0;
    } else {
      L_get_registry_by_ref (L, video_mode_ref);
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
  lua_pushstring (L, "MININIM VIDEO INTERFACE");
  return 1;
}
END_LUA

bool
L_push_video_routine (lua_State *L)
{
  L_get_registry_by_ref (L, video_mode_ref);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushstring (L, video_mode);
  lua_rawget (L, -2);
  lua_remove (L, -2);

  if (! lua_isfunction (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  return true;
}
