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

lua_State *main_L;
ALLEGRO_MUTEX *L_mutex;

static int table_getn_ref = LUA_NOREF;
static int weak_registry_ref = LUA_NOREF;

int
load_script (const char *filename)
{
  assert (main_L);
  int e = luaL_loadfile (main_L, filename);
  return e == LUA_ERRFILE;
}

void
init_script (void)
{
  /* setup canonical environment */
  lua_State *L = lua_open ();
  main_L = L;

  assert (L);

  luaopen_base (L);
  luaopen_table (L);
  luaopen_io (L);
  luaopen_string (L);
  luaopen_math (L);
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
  define_L_mininim (L);

  lua_newuserdata (L, 0);
  luaL_getmetatable (L, L_MININIM);
  lua_setmetatable (L, -2);
  lua_setglobal (L, "MININIM");

  /* table.getn reference */
  lua_pushstring(L, "table");
  lua_gettable(L, LUA_GLOBALSINDEX);
  lua_pushstring(L, "getn");
  lua_gettable(L, -2);
  L_set_registry_by_ref (L, &table_getn_ref);

  /* weak registry */
  lua_newtable (L);
  lua_pushstring (L, "__mode");
  lua_pushstring (L, "v");
  lua_rawset (L, -3);
  lua_pushvalue (L, -1);
  lua_setmetatable (L, -2);
  L_set_registry_by_ref (L, &weak_registry_ref);

  /* REPL */
  L_mutex = al_create_mutex_recursive ();
  repl_cond = al_create_cond ();
  debug_cond = al_create_cond ();
  lock_lua ();
  repl_L = lua_newthread (L);
  L_set_registry_by_ref (L, &repl_thread_ref);
  repl_thread = al_create_thread (repl, repl_L);
  al_start_thread (repl_thread);
  al_wait_cond (repl_cond, L_mutex);

  /* load script */
  int e = load_resource ("data/script/mininim.lua",
                         (load_resource_f) load_script, false);

  if (! e) {
    /* run-time error */
    if (lua_isfunction (L, -1)) L_call (L, 0, 0);
    /* syntax error */
    else error (-1, 0, "%s", lua_tostring(L, -1));
  } else {
    /* no script found */
    lua_remove (L, 1);
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
  al_set_thread_should_stop (repl_thread);
  unlock_lua ();

  al_rest (0.1);

  /* These sometimes cause trouble */
  /* al_join_thread (repl_thread, NULL); */
  al_destroy_thread (repl_thread);

  al_destroy_mutex (L_mutex);
  al_destroy_cond (repl_cond);
  al_destroy_cond (debug_cond);
  lua_close (main_L);
}

void *
L_check_type (lua_State *L, int index, const char *tname)
{
  void *ud = luaL_checkudata (L, index, tname);
  if (! ud) L_error_expected_got (L, index, tname);
  return ud;
}

int
L_error_expected_got (lua_State *L, int index, const char *expected_tname)
{
  char *got_tname = L_typename (L, index);
  lua_pushfstring (L, "%s expected, got %s", expected_tname, got_tname);
  al_free (got_tname);
  return lua_error (L);
}

bool
L_call (lua_State *L, int nargs, int nresults)
{
  int base = lua_gettop(L) - nargs;
  lua_pushcfunction (L, L_TRACEBACK);
  lua_insert (L, base);
  if (lua_pcall (L, nargs, nresults, base)) {
    if (repl_prompt_ready) fprintf (stderr, "\n");
    fprintf (stderr, "%s\n", lua_tostring(L, -1));
    lua_pop (L, 1);
    lua_remove(L, base);
    int i;
    for (i = 0; i < nresults; i++) lua_pushnil (L);
    return false;
  } else {
    lua_remove(L, base);
    return true;
  }
}

bool
L_run_hook (lua_State *L)
{
  if (lua_isfunction (L, -1)) {
    bool r = L_call (L, 0, 0);
    return r;
  } else {
    lua_pop (L, 1);
    return true;
  }
}

void
L_set_registry_by_ptr (lua_State *L, void *p)
{
  lua_pushlightuserdata (L, p);
  lua_insert (L, -2);
  lua_rawset (L, LUA_REGISTRYINDEX);
}

void
L_get_registry_by_ptr (lua_State *L, void *p)
{
  lua_pushlightuserdata (L, p);
  lua_rawget (L, LUA_REGISTRYINDEX);
}

int
L_set_registry_by_ref (lua_State *L, int *r)
{
  luaL_unref (L, LUA_REGISTRYINDEX, *r);
  *r = luaL_ref (L, LUA_REGISTRYINDEX);
  return *r;
}

void
L_get_registry_by_ref (lua_State *L, int r)
{
  lua_rawgeti (L, LUA_REGISTRYINDEX, r);
}

void
L_set_weak_registry_by_ptr (lua_State *L, void *p)
{
  L_get_registry_by_ref (L, weak_registry_ref);
  lua_pushlightuserdata (L, p);
  lua_pushvalue (L, -3);
  lua_rawset (L, -3);
  lua_pop (L, 2);
}

void
L_get_weak_registry_by_ptr (lua_State *L, void *p)
{
  L_get_registry_by_ref (L, weak_registry_ref);
  lua_pushlightuserdata (L, p);
  lua_rawget (L, -2);
  lua_remove (L, -2);
}

void
L_gc (lua_State *L)
{
  int count = lua_getgccount (L);
  int threshold = lua_getgcthreshold (L);
  if (count < threshold) return;
  palette_cache_gc (L);
}

void
L_push_interface (lua_State *L, const char *tname)
{
  lua_newuserdata (L, 0);
  luaL_getmetatable (L, tname);
  lua_setmetatable (L, -2);
}

int
lua_getn (lua_State *L, int index)
{
  index = lua_abs_index (L, index);
  if (lua_type (L, index) != LUA_TTABLE)
    return 0;
  L_get_registry_by_ref (L, table_getn_ref);
  lua_pushvalue (L, index);
  lua_call (L, 1, 1);
  int nmemb = lua_tonumber (L, -1);
  lua_pop (L, 1);
  return nmemb;
}

lua_Number
L_rawgeti_tonumber (lua_State *L, int index, int n)
{
  index = lua_abs_index (L, index);
  lua_rawgeti (L, index, n);
  lua_Number r = lua_tonumber (L, -1);
  lua_pop (L, 1);
  return r;
}

void
lock_lua ()
{
  al_lock_mutex (L_mutex);
  al_set_target_backbuffer (display);
}

void
unlock_lua ()
{
  al_set_target_bitmap (NULL);
  al_unlock_mutex (L_mutex);
}

void
L_set_string_var (lua_State *L, int index, char **var)
{
  index = lua_abs_index (L, index);
  const char *value = lua_tostring (L, index);
  if (value) set_string_var (var, value);
}
