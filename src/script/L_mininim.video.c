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

static int color (lua_State *L);
static int __color_eq (lua_State *L);
static void L_push_color (lua_State *L, ALLEGRO_COLOR c);
static ALLEGRO_COLOR L_tocolor (lua_State *L, int index);

static void L_push_palette (lua_State *L, char *palette_name);
static ALLEGRO_COLOR L_palette (ALLEGRO_COLOR c);

static int load (lua_State *L);

static bool L_get_base_property (lua_State *L, char *object);
static bool L_get_bitmap_property (lua_State *L, char *object, int index,
                                   char *key);

void
define_L_mininim_video (lua_State *L)
{
  /* mininim.video */
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

  lua_pop (L, 1);

  /* video mode table */
  lua_newtable (L);
  L_set_registry (L, &video_mode_ref);

  /* mininim.video.color */
  luaL_newmetatable(L, "mininim.video.color");

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __color_eq);
  lua_rawset (L, -3);

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
    } else if (! strcasecmp (key, "color")) {
      lua_pushcfunction (L, color);
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
      L_set_string_var (L, 3, "mininim.video.current", &video_mode);
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
  ALLEGRO_BITMAP **b = luaL_checkudata (L, -1, "mininim.video[?][?]");
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
L_push_color (lua_State *L, ALLEGRO_COLOR c)
{
  ALLEGRO_COLOR *c_new = lua_newuserdata (L, sizeof (*c_new));
  luaL_getmetatable (L, "mininim.video.color");
  lua_setmetatable (L, -2);
  *c_new = c;
}

ALLEGRO_COLOR
L_tocolor (lua_State *L, int index)
{
  return * (ALLEGRO_COLOR *) lua_touserdata (L, index);
}

int
color (lua_State *L)
{
  if (lua_isnumber (L, 1)) {
    int r = lua_tonumber (L, 1);
    int g = lua_tonumber (L, 2);
    int b = lua_tonumber (L, 3);
    ALLEGRO_COLOR c = al_map_rgb (r, g, b);
    L_push_color (L, c);
    return 1;
  } else if (lua_isstring (L, 1)) {
    const char *string = lua_tostring (L, 1);
    ALLEGRO_COLOR c = al_color_name (string);
    if (color_eq (c, al_map_rgb (0, 0, 0)))
      c = al_color_html (string);
    L_push_color (L, c);
    return 1;
  } else return luaL_argerror (L, 1, "must be number or string");
}

int
__color_eq (lua_State *L)
{
  ALLEGRO_COLOR c0 = L_tocolor (L, 1);
  ALLEGRO_COLOR c1 = L_tocolor (L, 2);
  lua_pushboolean (L, color_eq (c0, c1));
  return 1;
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
  lua_pushvalue (L, -1);
  L_push_color (L, c);
  L_call (L, 1, 1, 0);
  ALLEGRO_COLOR c_ret = L_tocolor (L, -1);
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
