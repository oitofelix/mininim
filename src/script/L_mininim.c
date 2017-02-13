/*
  L_mininim.c -- mininim script module;

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

static const char load_assets_hook_key = 'k';

static int __index (lua_State *L);
static int __newindex (lua_State *L);

void
define_L_mininim (lua_State *L)
{
  luaL_newmetatable(L, "mininim");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pop (L, 1);

  /* mininim.level */
  define_L_mininim_level (L);

  /* mininim.actor */
  define_L_mininim_actor (L);

  /* mininim.audio */
  define_L_mininim_audio (L);

  /* mininim.setting */
  define_L_mininim_setting (L);
}

void
run_load_assets_hook (void)
{
  L_get_registry (L, &load_assets_hook_key);
  L_run_hook (L);
}

int
__index (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "cycle")) {
      lua_pushnumber (L, anim_cycle);
      return 1;
    } else if (! strcasecmp (key, "load_assets_hook")) {
      L_get_registry (L, &load_assets_hook_key);
      return 1;
    } else if (! strcasecmp (key, "level")) {
      L_push_interface (L, "mininim.level");
      return 1;
    } else if (! strcasecmp (key, "actor")) {
      L_push_interface (L, "mininim.actor");
      return 1;
    } else if (! strcasecmp (key, "audio")) {
      L_push_interface (L, "mininim.audio");
      return 1;
    } else if (! strcasecmp (key, "setting")) {
      L_push_interface (L, "mininim.setting");
      return 1;
    } else return L_error_invalid_key_string (L, key, "mininim");
  default: return L_error_invalid_key_type (L, type, "mininim");
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
    if (! strcasecmp (key, "load_assets_hook")) {
      if (lua_isfunction (L, -1))
        L_set_registry (L, &load_assets_hook_key);
      else
        return luaL_error (L, "mininim.load_assets_hook must be a function");
      return 0;
    } else return L_error_invalid_key_string (L, key, "mininim");
  default: return L_error_invalid_key_type (L, type, "mininim");
  }
}
