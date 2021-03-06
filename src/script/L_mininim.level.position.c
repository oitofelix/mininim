/*
  L_mininim.level.position.c -- mininim.level.position script module;

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

char *wall_correlation_string (enum wall_correlation wc);

static DECLARE_LUA (activate);

void
define_L_mininim_level_position (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_LEVEL_POSITION);

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
L_pushposition (lua_State *L, struct pos *p)
{
  struct pos *p_new = lua_newuserdata (L, sizeof (*p_new));
  luaL_getmetatable (L, L_MININIM_LEVEL_POSITION);
  lua_setmetatable (L, -2);
  *p_new = *p;
}

BEGIN_LUA (L_mininim_level_position)
{
  int room = luaL_checknumber (L, 1);
  int floor = luaL_checknumber (L, 2);
  int place = luaL_checknumber (L, 3);
  struct pos p;
  new_pos (&p, &global_level, room, floor, place);
  L_pushposition (L, &p);
  return 1;
}
END_LUA

char *
wall_correlation_string (enum wall_correlation wc)
{
  switch (wc) {
  case SWS: return "SWS";
  case SWW: return "SWW";
  case WWS: return "WWS";
  case WWW: return "WWW";
  default: return "NONE";
  }
}

BEGIN_LUA (__eq)
{
  struct pos *p0 = luaL_checkudata (L, 1, L_MININIM_LEVEL_POSITION);
  struct pos *p1 = luaL_checkudata (L, 2, L_MININIM_LEVEL_POSITION);
  if (p0 && p1) lua_pushboolean (L, peq (p0, p1));
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  struct pos *p = luaL_checkudata (L, 1, L_MININIM_LEVEL_POSITION);

  if (! p) {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "room")) {
      lua_pushnumber (L, p->room);
      return 1;
    } else if (! strcasecmp (key, "floor")) {
      lua_pushnumber (L, p->floor);
      return 1;
    } else if (! strcasecmp (key, "place")) {
      lua_pushnumber (L, p->place);
      return 1;
    } else if (! strcasecmp (key, "normal")) {
      struct pos np; npos (p, &np);
      L_pushposition (L, &np);
      return 1;
    } else if (! strcasecmp (key, "wall_correlation")) {
      enum wall_correlation wc = wall_correlation (p);
      char * wc_str = wall_correlation_string (wc);
      lua_pushstring (L, wc_str);
      return 1;
    } else if (! strcasecmp (key, "activate")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, activate, 1);
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
  struct pos *p = luaL_checkudata (L, 1, L_MININIM_LEVEL_POSITION);

  if (! p) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "room")) {
      p->room = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "floor")) {
      p->floor = lua_tonumber (L, 3);
      return 0;
    } else if (! strcasecmp (key, "place")) {
      p->place = lua_tonumber (L, 3);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  struct pos *p = luaL_checkudata (L, 1, L_MININIM_LEVEL_POSITION);
  lua_pushfstring (L, L_MININIM_LEVEL_POSITION " (%d, %d, %d)",
                   p ? p->room : -1, p ? p->floor : -1, p ? p->place : -1);
  return 1;
}
END_LUA

BEGIN_LUA (activate)
{
  struct pos *p =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_LEVEL_POSITION);
  if (p && is_valid_pos (p)) activate_con (p);
  return 0;
}
END_LUA
