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

static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int __tostring (lua_State *L);

void
define_L_mininim_level (lua_State *L)
{
  luaL_newmetatable (L, L_MININIM_LEVEL);

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

  /* mininim.level.position */
  define_L_mininim_level_position (L);
}

void
run_level_start_hook (lua_State *L)
{
  L_get_registry (L, level_start_hook_ref);
  L_run_hook (L);
}

void
run_level_cycle_hook (lua_State *L)
{
  L_get_registry (L, level_cycle_hook_ref);
  L_run_hook (L);
}

int
__eq (lua_State *L)
{
  lua_pushboolean (L, true);
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
    if (! strcasecmp (key, "number")) {
      lua_pushnumber (L, global_level.n);
      return 1;
    } else if (! strcasecmp (key, "start_hook")) {
      L_get_registry (L, level_start_hook_ref);
      return 1;
    } else if (! strcasecmp (key, "cycle_hook")) {
      L_get_registry (L, level_cycle_hook_ref);
      return 1;
    } else if (! strcasecmp (key, "position")) {
      lua_pushcfunction (L, L_mininim_level_position);
      return 1;
    } else if (! strcasecmp (key, "retry")) {
      lua_pushboolean (L, retry_level == global_level.n
                       && replay_mode == NO_REPLAY);
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
    if (! strcasecmp (key, "start_hook")) {
      L_set_registry (L, &level_start_hook_ref);
      return 0;
    } else if (! strcasecmp (key, "cycle_hook")) {
      L_set_registry (L, &level_cycle_hook_ref);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  lua_pushfstring (L, "MININIM LEVEL %d INTERFACE", global_level.n);
  return 1;
}
