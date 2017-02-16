/*
  L_mininim.level_con.c -- mininim.level[{?, ?, ?}] script module;

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

static int activate (lua_State *L);

void
define_L_mininim_level_position (lua_State *L)
{
  luaL_newmetatable(L, "mininim.level.position");

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
L_pushpos (lua_State *L, struct pos *p)
{
  struct pos *p_new = lua_newuserdata (L, sizeof (*p_new));
  luaL_getmetatable (L, "mininim.level.position");
  lua_setmetatable (L, -2);
  *p_new = *p;
}

int
L_mininim_level_position (lua_State *L)
{
  int room = luaL_checknumber (L, 1);
  int floor = luaL_checknumber (L, 2);
  int place = luaL_checknumber (L, 3);
  struct pos p;
  new_pos (&p, &global_level, room, floor, place);
  L_pushpos (L, &p);
  return 1;
}

int
__eq (lua_State *L)
{
  struct pos *p0 = lua_touserdata (L, 1);
  struct pos *p1 = lua_touserdata (L, 2);
  lua_pushboolean (L, peq (p0, p1));
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
    if (! strcasecmp (key, "activate")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, activate, 1);
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
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    break;
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  struct pos *p = lua_touserdata (L, 1);
  lua_pushfstring (L, "mininim.level.position (%d, %d, %d)",
                   p->room, p->floor, p->place);
  return 1;
}

int
activate (lua_State *L)
{
  struct pos *p = lua_touserdata (L, lua_upvalueindex (1));
  activate_con (p);
  return 0;
}
