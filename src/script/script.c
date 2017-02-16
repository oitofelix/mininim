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

  luaopen_base (L);
  luaopen_table (L);
  luaopen_io (L);
  luaopen_string (L);
  luaopen_math (L);
  luaopen_debug (L);
  luaopen_loadlib (L);

  /* path */
  lua_settop (L, 0);
  lua_pushfstring (L, "%sdata/?.lua;"
                   "%sdata/?.lua;"
                   "%sdata/?.lua;"
                   "%sdata/?.lua;"
                   "%sdata/?.lua;",
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

  int e = load_resource ("data/script/mininim.lua",
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

  /* REPL */
  lua_settop (L, 0);
  repl_mutex = al_create_mutex_recursive ();
  al_lock_mutex (repl_mutex);
  repl_L = lua_newthread (L);
  L_set_registry (L, &repl_thread_ref);
  repl_thread = al_create_thread (repl, repl_L);
  al_start_thread (repl_thread);
}

void
finalize_script (void)
{
  al_set_thread_should_stop (repl_thread);
  al_unlock_mutex (repl_mutex);
  al_join_thread (repl_thread, NULL);
  al_destroy_thread (repl_thread);
  repl_thread = NULL;
  al_destroy_mutex (repl_mutex);
  lua_close (L);
}

void *
L_check_type (lua_State *L, int index, const char *tname)
{
  void *ud = luaL_checkudata (L, index, tname);
  if (! ud) luaL_error (L, "'%s' expected", tname);
  return ud;
}

bool
L_call (lua_State *L, int nargs, int nresults, int errfunc)
{
  if (lua_pcall (L, nargs, nresults, errfunc)) {
    error (0, 0, "%s", lua_tostring(L, -1));
    lua_pop (L, 1);
    return false;
  } else return true;
}

bool
L_run_hook (lua_State *L)
{
  if (lua_isfunction (L, -1)) return L_call (L, 0, 0, 0);
  else {
    lua_pop (L, 1);
    return true;
  }
}

int
L_set_registry (lua_State *L, int *r)
{
  luaL_unref (L, LUA_REGISTRYINDEX, *r);
  *r = luaL_ref (L, LUA_REGISTRYINDEX);
  return *r;
}

void
L_get_registry (lua_State *L, int r)
{
  lua_rawgeti (L, LUA_REGISTRYINDEX, r);
}

void
L_push_interface (lua_State *L, const char *tname)
{
  lua_newuserdata (L, 0);
  luaL_getmetatable (L, tname);
  lua_setmetatable (L, -2);
}
