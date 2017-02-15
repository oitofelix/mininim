/*
  L_mininim.level.c -- mininim.level script module;

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

static int level_start_hook_ref = LUA_NOREF;
static int level_cycle_hook_ref = LUA_NOREF;

static int __index (lua_State *L);
static int __newindex (lua_State *L);

static bool L_is_valid_pos (lua_State *L, int index, struct pos *p);

void
define_L_mininim_level (lua_State *L)
{
  luaL_newmetatable (L, "mininim.level");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.level");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pop (L, 1);

  /* mininim.level[{?, ?, ?}] */
  define_L_mininim_level_con (L);
}

void
run_level_start_hook (void)
{
  L_get_registry (L, level_start_hook_ref);
  L_run_hook (L);
}

void
run_level_cycle_hook (void)
{
  L_get_registry (L, level_cycle_hook_ref);
  L_run_hook (L);
}

bool
L_is_valid_pos (lua_State *L, int index, struct pos *p)
{
  /* check room */
  lua_rawgeti (L, index, 1);
  if (! lua_isnumber (L, -1)) goto invalid;
  lua_Number room = lua_tonumber (L, -1);
  if (room <= 0) goto invalid;
  lua_pop (L, 1);

  /* check floor */
  lua_rawgeti (L, index, 2);
  if (! lua_isnumber (L, -1)) goto invalid;
  lua_Number floor = lua_tonumber (L, -1);
  lua_pop (L, 1);

  /* check place */
  lua_rawgeti (L, index, 3);
  if (! lua_isnumber (L, -1)) goto invalid;
  lua_Number place = lua_tonumber (L, -1);
  lua_pop (L, 1);

  if (p) new_pos (p, &global_level, room, floor, place);

  return true;

 invalid:
  lua_pop (L, 1);
  return false;
}

void
L_push_pos (lua_State *L, struct pos *p)
{
  lua_newtable (L);

  /* room */
  lua_pushnumber (L, p->room);
  lua_rawseti (L, -2, 1);

  /* floor */
  lua_pushnumber (L, p->floor);
  lua_rawseti (L, -2, 1);

  /* place */
  lua_pushnumber (L, p->place);
  lua_rawseti (L, -2, 1);
}

int
__index (lua_State *L)
{
  struct pos p;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "number")) {
      lua_pushnumber (L, global_level.n);
      return 1;
    } else if (! strcasecmp (key, "start_hook")) {
      L_get_registry (L, level_start_hook_ref);
      return 1;
    } else if (! strcasecmp (key, "cycle_hook")) {
      L_get_registry (L, level_cycle_hook_ref);
      return 1;
    } else if (! strcasecmp (key, "retry")) {
      lua_pushboolean (L, retry_level == global_level.n
                       && replay_mode == NO_REPLAY);
      return 1;
    } else return L_error_invalid_key_string (L, key, "mininim.level");
  case LUA_TTABLE:
    if (! L_is_valid_pos (L, 2, &p)) return luaL_error (L, "invalid position");
    struct pos *ud = lua_newuserdata (L, sizeof (*ud));
    luaL_getmetatable (L, "mininim.level[{?, ?, ?}]");
    lua_setmetatable (L, -2);
    *ud = p;
    return 1;
  default: return L_error_invalid_key_type (L, type, "mininim.level");
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
    if (! strcasecmp (key, "number"))
      return luaL_error (L, "mininim.level.number is read-only");
    else if (! strcasecmp (key, "start_hook")) {
      if (lua_isfunction (L, -1))
        L_set_registry (L, &level_start_hook_ref);
      else
        return luaL_error (L, "mininim.level.start_hook must be a function");
      return 0;
    } else if (! strcasecmp (key, "cycle_hook")) {
      if (lua_isfunction (L, -1))
        L_set_registry (L, &level_cycle_hook_ref);
      else
        return luaL_error (L, "mininim.level.cycle_hook must be a function");
      return 0;
    } else return L_error_invalid_key_string (L, key, "mininim.level");
  default: return L_error_invalid_key_type (L, type, "mininim.level");
  }
}
