/*
  L_mininim.video.color.c -- mininim.video.color script module;

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

static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int __tostring (lua_State *L);

void
define_L_mininim_video_color (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_COLOR);

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
}

void
L_pushcolor (lua_State *L, ALLEGRO_COLOR c)
{
  ALLEGRO_COLOR *c_new = lua_newuserdata (L, sizeof (*c_new));
  luaL_getmetatable (L, L_MININIM_VIDEO_COLOR);
  lua_setmetatable (L, -2);
  *c_new = c;
}

int
L_mininim_video_color (lua_State *L)
{
  if (lua_isnumber (L, 1)) {
    int r = lua_tonumber (L, 1);
    int g = luaL_checknumber (L, 2);
    int b = luaL_checknumber (L, 3);
    ALLEGRO_COLOR c = al_map_rgb (r, g, b);
    L_pushcolor (L, c);
    return 1;
  } else if (lua_isstring (L, 1)) {
    const char *string = lua_tostring (L, 1);
    ALLEGRO_COLOR c = al_color_name (string);
    if (color_eq (c, al_map_rgb (0, 0, 0)))
      c = al_color_html (string);
    L_pushcolor (L, c);
    return 1;
  } else return luaL_argerror (L, 1, "must be number or string");
}

int
__eq (lua_State *L)
{
  ALLEGRO_COLOR *c0 = luaL_checkudata (L, 1, L_MININIM_VIDEO_COLOR);
  ALLEGRO_COLOR *c1 = luaL_checkudata (L, 2, L_MININIM_VIDEO_COLOR);
  if (c0 && c1) lua_pushboolean (L, color_eq (*c0, *c1));
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}

int
__index (lua_State *L)
{
  ALLEGRO_COLOR *c_ptr = luaL_checkudata (L, 1, L_MININIM_VIDEO_COLOR);

  ALLEGRO_COLOR c;
  if (c_ptr) c = *c_ptr;
  else {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "r")) {
      unsigned char r, g, b;
      al_unmap_rgb (c, &r, &g, &b);
      lua_pushnumber (L, r);
      return 1;
    } else if (! strcasecmp (key, "g")) {
      unsigned char r, g, b;
      al_unmap_rgb (c, &r, &g, &b);
      lua_pushnumber (L, g);
      return 1;
    } else if (! strcasecmp (key, "b")) {
      unsigned char r, g, b;
      al_unmap_rgb (c, &r, &g, &b);
      lua_pushnumber (L, b);
      return 1;
    } else if (! strcasecmp (key, "html")) {
      unsigned char r, g, b;
      al_unmap_rgb (c, &r, &g, &b);
      char html[8];
      al_color_rgb_to_html (r / 255.0, g / 255.0, b / 255.0, html);
      lua_pushstring (L, html);
      return 1;
    } else if (! strcasecmp (key, "name")) {
      unsigned char r, g, b;
      al_unmap_rgb (c, &r, &g, &b);
      char const *name =
        al_color_rgb_to_name (r / 255.0, g / 255.0, b / 255.0);
      lua_pushstring (L, name);
      return 1;
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}

int
__newindex (lua_State *L)
{
  ALLEGRO_COLOR *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COLOR);
  if (! c) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "r")) {
      if (! lua_isnumber (L, 3)) return 0;
      unsigned char r, g, b;
      al_unmap_rgb (*c, &r, &g, &b);
      r = lua_tonumber (L, 3);
      *c = al_map_rgb (r, g, b);
      return 0;
    } else if (! strcasecmp (key, "g")) {
      if (! lua_isnumber (L, 3)) return 0;
      unsigned char r, g, b;
      al_unmap_rgb (*c, &r, &g, &b);
      g = lua_tonumber (L, 3);
      *c = al_map_rgb (r, g, b);
      return 0;
    } else if (! strcasecmp (key, "b")) {
      if (! lua_isnumber (L, 3)) return 0;
      unsigned char r, g, b;
      al_unmap_rgb (*c, &r, &g, &b);
      b = lua_tonumber (L, 3);
      *c = al_map_rgb (r, g, b);
      return 0;
    } else if (! strcasecmp (key, "html")) {
      if (lua_isstring (L, 3))
        *c = al_color_html (lua_tostring (L, 3));
      else return 0;
    } else if (! strcasecmp (key, "name")) {
      if (lua_isstring (L, 3))
        *c = al_color_name (lua_tostring (L, 3));
      else return 0;
    } else break;
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  ALLEGRO_COLOR *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COLOR);
  if (c) {
    unsigned char r, g, b;
    al_unmap_rgb (*c, &r, &g, &b);
    lua_pushfstring (L, L_MININIM_VIDEO_COLOR " (%d, %d, %d)", r, g, b);
  } else lua_pushstring (L, L_MININIM_VIDEO_COLOR " (-1, -1, -1)");
  return 1;
}
