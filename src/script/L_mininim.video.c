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

static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int __tostring (lua_State *L);

static void L_push_palette (lua_State *L, char *palette_name);
static ALLEGRO_COLOR L_palette (ALLEGRO_COLOR c);

static bool L_get_base_property (lua_State *L, char *object);
static bool L_get_bitmap_property (lua_State *L, char *object, int index,
                                   char *key);

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
  L_set_registry (L, &video_mode_ref);

  /* mininim.video.bitmap */
  define_L_mininim_video_bitmap (L);

  /* mininim.video.color */
  define_L_mininim_video_color (L);
}

int
__eq (lua_State *L)
{
  lua_pushboolean (L, true);
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
    if (! strcasecmp (key, "bitmap")) {
      lua_pushcfunction (L, L_mininim_video_bitmap);
      return 1;
    } else if (! strcasecmp (key, "color")) {
      lua_pushcfunction (L, L_mininim_video_color);
      return 1;
    } else if (! strcasecmp (key, "palette_cache_size_limit")) {
      lua_pushnumber (L, palette_cache_size_limit);
      return 1;
    } else if (! strcasecmp (key, "current")) {
      assert (video_mode);
      L_get_registry (L, video_mode_ref);
      lua_pushstring (L, video_mode);
      lua_gettable (L, 1);
      return 1;
    } else {
      L_get_registry (L, video_mode_ref);
      lua_replace (L, 1);
      lua_gettable (L, 1);
      return 1;
    }
  default: break;
  }

  lua_pushnil (L);
  return 1;
}

int
__newindex (lua_State *L)
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
      L_get_registry (L, video_mode_ref);
      lua_replace (L, 1);
      lua_settable (L, 1);
      return 0;
    }
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  lua_pushstring (L, "MININIM VIDEO INTERFACE");
  return 1;
}

bool
L_get_base_property (lua_State *L, char *object)
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

  return true;
}

bool
L_get_bitmap_property (lua_State *L, char *object, int index, char *key)
{
  if (! L_get_base_property (L, object)) return false;

  if (! lua_istable (L, -1)) {
    lua_pop (L, 1);
    return false;
  }

  char *actual_env_mode;
  if (! strcasecmp (env_mode, "ORIGINAL"))
    actual_env_mode = em == PALACE ? "PALACE" : "DUNGEON";
  else actual_env_mode = env_mode;

  lua_pushstring (L, actual_env_mode);
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
L_bitmap (lua_State *L, char *object, int index)
{
  if (! L_get_bitmap_property (L, object, index, "bitmap")) return NULL;
  ALLEGRO_BITMAP **b = luaL_checkudata (L, -1, L_MININIM_VIDEO_BITMAP);
  lua_pop (L, 1);
  return b ? *b : NULL;
}

struct coord *
L_coord (lua_State *L, char *object, int index, struct pos *p,
         struct coord *c_ret)
{
  if (! L_get_bitmap_property (L, object, index, "coord")) return NULL;
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

void
L_push_palette (lua_State *L, char *palette_name)
{
  if (! L_get_base_property (L, palette_name)) {
    lua_pushnil (L);
    return;
  }
}

ALLEGRO_COLOR
L_palette (ALLEGRO_COLOR c)
{
  lua_State *L = main_L;
  lua_pushvalue (L, -1);
  L_pushcolor (L, c);
  ALLEGRO_COLOR c_ret;
  L_call (L, 1, 1, 0);
  ALLEGRO_COLOR *c_ptr = luaL_checkudata (L, -1, L_MININIM_VIDEO_COLOR);
  if (c_ptr) c_ret = *c_ptr;
  else c_ret = c;
  lua_pop (L, 1);
  return c_ret;
}

ALLEGRO_BITMAP *
L_apply_palette (lua_State *L, ALLEGRO_BITMAP *b, char *palette)
{
  ALLEGRO_BITMAP *r = b;

  L_push_palette (L, palette);
  if (lua_isfunction (L, -1))
    r = apply_palette_k (b, L_palette, lua_topointer (L, -1));
  lua_pop (L, 1);
  return r;
}
