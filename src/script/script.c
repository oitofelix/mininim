/*
  script.c -- script module;

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

lua_State *L;

int
load_script (const char *filename)
{
  assert (L);
  int e = luaL_loadfile (L, filename);
  return e == LUA_ERRFILE;
}

void
init_script (void)
{
  /* setup canonical environment */
  L = lua_open ();

  assert (L);

  luaopen_base(L);
  luaopen_table(L);
  luaopen_io(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_debug(L);

  /* path */
  lua_settop (L, 0);
  lua_pushfstring (L, "%sdata/scripts/?.lua;"
                   "%sdata/scripts/?.lua;"
                   "%sdata/scripts/?.lua;"
                   "%sdata/scripts/?.lua;"
                   "%sdata/scripts/?.lua;",
                   user_data_dir,
                   data_dir ? data_dir : "",
                   "",
                   resources_dir,
                   system_data_dir);
  lua_setglobal (L, "LUA_PATH");

  /* mininim */
  lua_settop (L, 0);
  define_L_mininim (L);

  lua_newuserdata (L, 0);
  luaL_getmetatable (L, "mininim");
  lua_setmetatable (L, -2);
  lua_setglobal (L, "mininim");

  /* load script */
  lua_settop (L, 0);

  int e = load_resource ("data/scripts/mininim.lua",
                         (load_resource_f) load_script, false);

  if (! e) {
    if (lua_isfunction (L, -1)) {
      if (lua_pcall (L, 0, 0, 0))
        error (-1, 0, "%s", lua_tostring(L, -1));
    }
    else error (-1, 0, "%s", lua_tostring(L, -1));
  } else {
    while (lua_gettop (L)) {
      error (0, 0, "%s", lua_tostring(L, 1));
      lua_remove (L, 1);
    }
    exit (-1);
  }

  lua_settop (L, 0);
}

void
finalize_script (void)
{
  lua_close (L);
}

void *
L_check_type (lua_State *L, int index, const char *tname)
{
  void *ud = luaL_checkudata (L, index, tname);
  if (! ud) luaL_error (L, "'%s' expected", tname);
  return ud;
}

int
L_error_invalid_key_string (lua_State *L, const char *key, const char *tname)
{
  return luaL_error (L, "\"%s\" is not a valid key for '%s'", key, tname);
}

int
L_error_invalid_key_type (lua_State *L, int type, const char *tname)
{
  return luaL_error (L, "invalid key type '%s' for '%s'",
                     lua_typename (L, type), tname);
}

int
L__tostring (lua_State *L)
{
  lua_pushvalue (L, lua_upvalueindex (1));
  return 1;
}

bool
L_run_hook (lua_State *L)
{
  if (! lua_isnil (L, -1)) {
    if (lua_pcall (L, 0, 0, 0)) {
      error (0, 0, "%s", lua_tostring(L, -1));
      lua_pop (L, 1);
      return false;
    }
  }
  return true;
}

void
L_set_registry (lua_State *L, const char *key)
{
  lua_pushlightuserdata (L, (void *) key);
  lua_pushvalue (L, -2);
  lua_settable(L, LUA_REGISTRYINDEX);
  lua_pop (L, 1);
}

void
L_get_registry (lua_State *L, const char *key)
{
  lua_pushlightuserdata (L, (void *) key);
  lua_gettable (L, LUA_REGISTRYINDEX);
}

void
L_push_interface (lua_State *L, const char *tname)
{
  lua_newuserdata (L, 0);
  luaL_getmetatable (L, tname);
  lua_setmetatable (L, -2);
}
