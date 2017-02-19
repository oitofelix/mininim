/*
  L_mininim.video.bitmap.c -- mininim.video.bitmap script module;

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

static int L_apply_palette (lua_State *L);

void
define_L_mininim_video_bitmap (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_BITMAP);

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
L_pushbitmap (lua_State *L, ALLEGRO_BITMAP *b)
{
  if (b) {
    L_get_weak_registry_by_ptr (L, b);
    if (lua_isnil (L, -1)) {
      lua_pop (L, 1);
      ALLEGRO_BITMAP **b_new = lua_newuserdata (L, sizeof (*b_new));
      luaL_getmetatable (L, L_MININIM_VIDEO_BITMAP);
      lua_setmetatable (L, -2);
      *b_new = b;

      /* put output bitmap into weak registry for GC tracking */
      lua_pushvalue (L, -1);
      L_set_weak_registry_by_ptr (L, b);
    }
  } else lua_pushnil (L);
}

int
L_mininim_video_bitmap (lua_State *L)
{
  const char *filename = luaL_checkstring (L, 1);
  ALLEGRO_BITMAP *b = load_bitmap (filename);
  L_pushbitmap (L, b);
  return 1;
}

int
__gc (lua_State *L)
{
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  destroy_bitmap (*b);
  return 0;
}

int
__eq (lua_State *L)
{
  ALLEGRO_BITMAP **b0 = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  ALLEGRO_BITMAP **b1 = luaL_checkudata (L, 2, L_MININIM_VIDEO_BITMAP);
  if (b0 && b1) lua_pushboolean (L, *b0 == *b1);
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}

int
__index (lua_State *L)
{
  ALLEGRO_BITMAP **b_ptr = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);

  ALLEGRO_BITMAP *b;
  if (b_ptr) b = *b_ptr;
  else {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "apply_palette")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, L_apply_palette, 1);
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
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  lua_pushfstring (L, L_MININIM_VIDEO_BITMAP " %dx%d",
                   b ? al_get_bitmap_width (*b) : 0,
                   b ? al_get_bitmap_height (*b) : 0);
  return 1;
}

ALLEGRO_COLOR
L_palette (ALLEGRO_COLOR c)
{
  lua_State *L = main_L;
  lua_pushvalue (L, -1);
  L_pushcolor (L, c);
  ALLEGRO_COLOR c_ret;
  L_call (L, 1, 1);
  ALLEGRO_COLOR *c_ptr = luaL_checkudata (L, -1, L_MININIM_VIDEO_COLOR);
  if (c_ptr) c_ret = *c_ptr;
  else c_ret = c;
  lua_pop (L, 1);
  return c_ret;
}

int
L_apply_palette (lua_State *L)
{
  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) {
    lua_pushnil (L);
    return 1;
  }

  /* apply palette */
  luaL_checktype (L, 1, LUA_TFUNCTION);
  void *palette_ptr = (void *) lua_topointer (L, 1);
  ALLEGRO_BITMAP *r = apply_palette_k (*b, L_palette, palette_ptr);
  lua_pop (L, 1);

  /* push result */
  L_pushbitmap (L, r);
  return 1;
}
