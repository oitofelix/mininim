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

static int __index (lua_State *L);
static int __newindex (lua_State *L);

static int load (lua_State *L);

void
define_L_mininim_video (lua_State *L)
{
  luaL_newmetatable(L, "mininim.video");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.video");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_newtable (L);
  L_set_registry (L, &video_mode_ref);

  lua_pop (L, 1);

  /* mininim.video[?][?] */
  define_L_mininim_video_res (L);
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
      if (video_mode) {
        L_get_registry (L, video_mode_ref);
        lua_pushstring (L, video_mode);
        lua_gettable (L, 1);
      } else lua_pushnil (L);
      return 1;
    } else {
      L_get_registry (L, video_mode_ref);
      lua_replace (L, 1);
      lua_gettable (L, 1);
      return 1;
    }
  default: return L_error_invalid_key_type (L, type, "mininim.video");
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
      return luaL_error (L, "mininim.video.load is read-only");
    } else if (! strcasecmp (key, "current")) {
      L_set_video_mode (L, 3);
      return 0;
    } else {
      L_get_registry (L, video_mode_ref);
      lua_replace (L, 1);
      lua_settable (L, 1);
      return 0;
    }
  default: return L_error_invalid_key_type (L, type, "mininim.video");
  }
}

int
load (lua_State *L)
{
  const char *filename = luaL_checkstring (L, 1);

  ALLEGRO_BITMAP **b = lua_newuserdata (L, sizeof (*b));
  *b = load_bitmap (filename);

  if (*b) {
    luaL_getmetatable (L, "mininim.video[?][?]");
    lua_setmetatable (L, -2);
  } else {
    lua_pop (L, 1);
    lua_pushnil (L);
  }

  return 1;
}

bool
L_get (lua_State *L, char *object, enum em em, int index, char *key)
{
  L_get_registry (L, video_mode_ref);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushstring (L, video_mode);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushstring (L, object);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushstring (L, em == PALACE ? "PALACE" : "DUNGEON");
  lua_gettable (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushnumber (L, index);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  lua_pushstring (L, key);
  lua_gettable (L, -2);
  lua_remove (L, -2);

  return true;
}


ALLEGRO_BITMAP *
L_bitmap (lua_State *L, char *object, enum em em, int index)
{
  if (! L_get (L, object, em, index, "bitmap")) return NULL;
  ALLEGRO_BITMAP **b = luaL_checkudata (L, -1, "mininim.video[?][?]");
  lua_pop (L, 1);
  return b ? *b : NULL;
}

struct coord *
L_coord (lua_State *L, char *object, enum em em, int index, struct pos *p,
         struct coord *c_ret)
{
  if (! L_get (L, object, em, index, "coord")) return NULL;
  if (! lua_isfunction (L, -1)) {
    lua_pop (L, 1);
    return NULL;
  }

  lua_pushnumber (L, p->floor);
  lua_pushnumber (L, p->place);

  L_call (L, 2, 2, 0);

  new_coord (c_ret, p->l, p->room,
             lua_tonumber (L, 1), lua_tonumber (L, 2));

  lua_pop (L, 2);

  return c_ret;
}
