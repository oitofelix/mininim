/*
  L_mininim.video.rectangle.c -- mininim.video.rectangle script module;

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

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (draw);

void
define_L_mininim_video_rectangle (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_RECTANGLE);

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
L_pushrectangle (lua_State *L, struct rect *r)
{
  struct rect *r_new = lua_newuserdata (L, sizeof (*r_new));
  luaL_getmetatable (L, L_MININIM_VIDEO_RECTANGLE);
  lua_setmetatable (L, -2);
  *r_new = *r;
}

BEGIN_LUA (L_mininim_video_rectangle)
{
  struct coord *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COORDINATE);

  int x, y, w, h;

  if (c) {
    x = c->x;
    y = c->y;
    w = lua_tonumber (L, 2);
    h = luaL_checknumber (L, 3);
  } else {
    x = lua_tonumber (L, 1);
    y = lua_tonumber (L, 2);
    w = lua_tonumber (L, 3);
    h = lua_tonumber (L, 4);
  }

  struct rect r;
  new_rect (&r, room_view, x, y, w, h);
  L_pushrectangle (L, &r);
  return 1;
}
END_LUA

BEGIN_LUA (__eq)
{
  struct rect *r0 = luaL_checkudata (L, 1, L_MININIM_VIDEO_RECTANGLE);
  struct rect *r1 = luaL_checkudata (L, 2, L_MININIM_VIDEO_RECTANGLE);
  if (r0 && r1) {
    r0->c.room = room_view;
    r1->c.room = room_view;
    lua_pushboolean (L, rect_eq (r0, r1));
  } else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  struct rect *r = luaL_checkudata (L, 1, L_MININIM_VIDEO_RECTANGLE);

  if (! r) {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "coordinate")) {
      L_pushcoordinate (L, &r->c);
      return 1;
    } else if (! strcasecmp (key, "x")) {
      lua_pushnumber (L, r->c.x);
      return 1;
    } else if (! strcasecmp (key, "y")) {
      lua_pushnumber (L, r->c.y);
      return 1;
    } else if (! strcasecmp (key, "width")) {
      lua_pushnumber (L, r->w);
      return 1;
    } else if (! strcasecmp (key, "height")) {
      lua_pushnumber (L, r->h);
      return 1;
    } else if (! strcasecmp (key, "draw")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, draw, 1);
      return 1;
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}
END_LUA

BEGIN_LUA (__newindex)
{
  struct rect *r = luaL_checkudata (L, 1, L_MININIM_VIDEO_RECTANGLE);

  if (! r) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "coordinate")) {
      struct coord *c =luaL_checkudata (L, 3, L_MININIM_VIDEO_COORDINATE);
      if (! c) return 0;
      r->c = *c;
      return 0;
    } else if (! strcasecmp (key, "x")) {
      r->c.x = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "y")) {
      r->c.y = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "width")) {
      r->w = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "height")) {
      r->h = lua_tonumber (L, 3);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  struct rect *r = luaL_checkudata (L, 1, L_MININIM_VIDEO_RECTANGLE);
  lua_pushfstring (L, L_MININIM_VIDEO_RECTANGLE " (%d, %d, %d, %d)",
                   r ? r->c.x : -1, r ? r->c.y : -1,
                   r ? r->w : -1, r ? r->h : -1);
  return 1;
}
END_LUA

BEGIN_LUA (draw)
{
  if (! L_target_bitmap) return 0;

  struct rect *r =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_RECTANGLE);

  if (! r) return 0;

  ALLEGRO_COLOR *c = L_check_type (L, 1, L_MININIM_VIDEO_COLOR);

  draw_filled_rect (L_target_bitmap, r, *c);

  return 0;
}
END_LUA
