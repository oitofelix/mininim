/*
  L_mininim.video.coordinate.c -- mininim.video.coordinate script module;

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

void
define_L_mininim_video_coordinate (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_COORDINATE);

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
L_pushcoordinate (lua_State *L, struct coord *c)
{
  struct coord *c_new = lua_newuserdata (L, sizeof (*c_new));
  luaL_getmetatable (L, L_MININIM_VIDEO_COORDINATE);
  lua_setmetatable (L, -2);
  *c_new = *c;
}

BEGIN_LUA (L_mininim_video_coordinate)
{
  int x = luaL_checknumber (L, 1);
  int y = luaL_checknumber (L, 2);
  struct coord c;
  new_coord (&c, &global_level, room_view, x, y);
  L_pushcoordinate (L, &c);
  return 1;
}
END_LUA

BEGIN_LUA (__eq)
{
  struct coord *c0 = luaL_checkudata (L, 1, L_MININIM_VIDEO_COORDINATE);
  struct coord *c1 = luaL_checkudata (L, 2, L_MININIM_VIDEO_COORDINATE);
  if (c0 && c1) {
    c0->room = room_view;
    c1->room = room_view;
    lua_pushboolean (L, coord_eq (c0, c1));
  }
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  struct coord *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COORDINATE);

  if (! c) {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "x")) {
      lua_pushnumber (L, c->x);
      return 1;
    } else if (! strcasecmp (key, "y")) {
      lua_pushnumber (L, c->y);
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
  struct coord *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COORDINATE);

  if (! c) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "x")) {
      c->x = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "y")) {
      c->y = lua_tonumber (L, 3);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  struct coord *c = luaL_checkudata (L, 1, L_MININIM_VIDEO_COORDINATE);
  lua_pushfstring (L, L_MININIM_VIDEO_COORDINATE " (%d, %d)",
                   c ? c->x : -1, c ? c->y : -1);
  return 1;
}
END_LUA
