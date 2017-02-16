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
static int __tostring (lua_State *L);

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

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

int
L_mininim_video_bitmap (lua_State *L)
{
  const char *filename = luaL_checkstring (L, 1);

  ALLEGRO_BITMAP **b = lua_newuserdata (L, sizeof (*b));
  *b = load_bitmap (filename);

  if (*b) {
    luaL_getmetatable (L, L_MININIM_VIDEO_BITMAP);
    lua_setmetatable (L, -2);
  } else {
    lua_pop (L, 1);
    lua_pushnil (L);
  }

  return 1;
}

int
__gc (lua_State *L)
{
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  if (b) destroy_bitmap (*b);
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
__tostring (lua_State *L)
{
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  lua_pushfstring (L, L_MININIM_VIDEO_BITMAP " %dx%d",
                   b ? al_get_bitmap_width (*b) : 0,
                   b ? al_get_bitmap_height (*b) : 0);
  return 1;
}
