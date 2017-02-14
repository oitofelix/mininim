/*
  L_mininim.video_res.c -- mininim.video[?][?] script module;

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

void
define_L_mininim_video_res (lua_State *L)
{
  luaL_newmetatable(L, "mininim.video[?][?]");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.video[?][?]");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

int
__gc (lua_State *L)
{
  ALLEGRO_BITMAP **b = lua_touserdata (L, 1);
  destroy_bitmap (*b);
  return 0;
}
