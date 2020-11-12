/*
  script.c -- script module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

static int table_getn_ref = LUA_NOREF;
static int weak_registry_ref = LUA_NOREF;


/************
 * Lua state
 ************/

/* Create Lua state and return it. */

lua_State *
create_lua_state ()
{
  /* Create Lua state with default panic function and allocator. */
  lua_State *L = luaL_newstate ();
  /* Ensure the state has been created. */
  if (! L)
    failerrno ("cannot create new Lua state");
  /* Create Lua extra space. */
  create_lua_extra_space (L);
  /* Make all standard Lua libraries available. */
  luaL_openlibs (L);
  /* Return Lua state just created. */
  return L;
}

/* Destroy Lua state L. */

void
destroy_lua_state (lua_State *L)
{
  /* Lua state L must be not NULL! */
  assert (L);
  /* Destroy Lua extra space structure. */
  destroy_lua_extra_space (main_L);
  /* Destroy Lua state L. */
  lua_close (L);
}


/******************
 * Lua extra space
 ******************/

/* Create extra space structure for Lua state L, store its pointer
   in the extra space of L and return that pointer. */

struct lua_extra_space *
create_lua_extra_space (lua_State *L)
{
  /* Lua state L must be not NULL! */
  assert (L);
  /* Get extra space pointer. */
  struct lua_extra_space **extra_space_ptr = lua_getextraspace (L);
  /* Extra space pointer must not be NULL! */
  assert (extra_space_ptr);
  /* Allocate extra space structure and put its pointer in the extra
     space of L. */
  szalloc (*extra_space_ptr);
  /* Create recursive mutex. */
  create_lua_mutex (*extra_space_ptr);
  /* Return extra space structure just created. */
  return *extra_space_ptr;
}

/* Destroy extra space structure of Lua state L. */

void
destroy_lua_extra_space (lua_State *L)
{
  /* Lua state L must be not NULL! */
  assert (L);
  /* Get extra space pointer. */
  struct lua_extra_space **extra_space_ptr = lua_getextraspace (L);
  /* Extra space pointer must not be NULL! */
  assert (extra_space_ptr);
  /* Destroy recursive mutex. */
  destroy_lua_mutex (*extra_space_ptr);
  /* Free extra space structure. */
  al_free (*extra_space_ptr);
  /* Erase extra space structure pointer. */
  *extra_space_ptr = NULL;
}

/* Return extra space structure pointer of Lua state L.  Asserted to
   be non-NULL. */

struct lua_extra_space *
get_lua_extra_space (lua_State *L)
{
  /* Lua state L must be not NULL! */
  assert (L);
  /* Get extra space pointer. */
  struct lua_extra_space **extra_space_ptr = lua_getextraspace (L);
  /* Pointer to extra space must not be NULL! */
  assert (extra_space_ptr);
  /* Pointer to extra space structure in extra space must not be
     NULL! */
  assert (*extra_space_ptr);
  /* Return it. */
  return *extra_space_ptr;
}


/**********************
 * Lua recursive mutex
 **********************/

/* Create recursive mutex for Lua extra space structure pointed by
   LES_PTR store it there and return that mutex. */

ALLEGRO_MUTEX *
create_lua_mutex (struct lua_extra_space *les_ptr)
{
  /* Lua extra space structure pointer LES_PTR must be not NULL!  */
  assert (les_ptr);
  /* Allocate recursive mutex. */
  ALLEGRO_MUTEX *mutex = al_create_mutex_recursive ();
  /* Ensure mutex has been successfully allocated. */
  if (! mutex)
    failerrno ("cannot create recursive mutex");
  /* Store MUTEX in the extra space structure and return it. */
  return les_ptr->mutex = mutex;
}

/* Destroy recursive mutex of Lua extra space structure pointed by
   LES_PTR. */

void
destroy_lua_mutex (struct lua_extra_space *les_ptr)
{
  /* Lua extra space structure pointer LES_PTR must be not NULL!  */
  assert (les_ptr);
  /* Get recursive mutex */
  ALLEGRO_MUTEX *mutex = les_ptr->mutex;
  /* MUTEX must not be NULL! */
  assert (mutex);
  /* Destroy recursive mutex. */
  al_destroy_mutex (mutex);
  /* Erase recursive mutex pointer. */
  les_ptr->mutex = NULL;
}

/* Return mutex of Lua state L.  Asserted to be non-NULL.  The mutex
   must have been previously allocated by alloc_lua_mutex. */

ALLEGRO_MUTEX *
get_lua_mutex (lua_State *L)
{
  /* Lua state L must not be NULL! */
  assert (L);
  /* Get mutex from extra space structure of L. */
  ALLEGRO_MUTEX *mutex = get_lua_extra_space (L)->mutex;
  /* Mutex must not be NULL! */
  assert (mutex);
  /* Return mutex. */
  return mutex;
}

/* Acquire lock of Lua state L. */

void
lock_lua (lua_State *L)
{
  al_lock_mutex (get_lua_mutex (L));
}

/* Release lock of Lua state L. */

void
unlock_lua (lua_State *L)
{
  al_unlock_mutex (get_lua_mutex (L));
}


/***********
 * Lua path
 ***********/

/* Add DIR path with the usual Lua package/module data directory
   suffixes (DIR/data/?.lua;DIR/data/?/init.lua;) to Lua string buffer
   DATA.  Intended for use with do_for_each_data_dir.  Return false as
   to not interrupt it.  See push_lua_path. */

bool
add_to_lua_path_buffer (void_luaL_Buffer *data,
			const char *dir)
{
  /* Lua string buffer pointer DATA and directory path DIR must not be
     NULL! */
  assert (data), assert (dir);
  /* DATA is a pointer to a Lua string buffer. */
  luaL_Buffer *b = data;
  /* add DIR/data/?.lua; */
  luaL_addstring (b, dir);
  luaL_addstring (b, PS"data"PS"?.lua;");
  /* add DIR/data/?/init.lua; */
  luaL_addstring (b, dir);
  luaL_addstring (b, PS"data"PS"?"PS"init.lua;");
  /* Tell do_for_each_data_dir to proceed to the next data
     directory. */
  return false;
}

/* Push Lua path string.  It lists load paths for all data directories
   known by do_for_each_data_dir.  It is intended to be used as a
   substring of package.path.  Return 1. */

int
push_lua_path (lua_State *L)
{
  /* Lua state L must not be NULL! */
  assert (L);
  /* Acquire Lua state lock. */
  lock_lua (L);
  /* Ensure stack size. */
  luaL_checkstack (L, 1, "cannot push Lua path");
  /* Initialize string buffer. */
  luaL_Buffer b; luaL_buffinit (L, &b);
  /* Add to path buffer load paths for each data directory.  Assert
     that all data directories are included. */
  assert (! do_for_each_data_dir (add_to_lua_path_buffer, &b));
  /* Push Lua path string. */
  luaL_pushresult (&b);
  /* Release Lua state lock. */
  unlock_lua (L);
  /* One value has been put on stack. */
  return 1;
}


/*******************
 * Table facilities
 *******************/

/* Remove field K from table at INDEX.  Ensure value at INDEX is a
   table.  Return 0. */

int
remove_table_field (lua_State *L, int index, const char *k)
{
  /* Lua state L and key K must not be NULL! */
  assert (L), assert (k);
  /* Acquire Lua state lock. */
  lock_lua (L);
  /* Ensure stack size. */
  luaL_checkstack (L, 1, "cannot remove field from table");
  /* Ensure INDEX is absolute */
  index = lua_absindex (L, index);
  /* Ensure value at INDEX is a table. */
  if (! lua_istable (L, index))
    return luaL_error (L, "attempt to remove field from non-table");
  /* Push nil value for the requested key. */
  lua_pushnil (L);
  /* Pop nil and assign it to INDEX[K]. */
  lua_setfield (L, index, k);
  /* Release Lua state lock. */
  unlock_lua (L);
  /* Put no values on stack. */
  return 0;
}

/* Return true if K is the last key of table at INDEX.  Otherwise,
   return false.  If K is NULL, effectively check whether table at
   INDEX is empty.  Ensure value at INDEX is a table. */

bool
is_last_table_key (lua_State *L, int index, const char *k)
{
  /* Lua state L must not be NULL! */
  assert (L);
  /* Acquire Lua state lock. */
  lock_lua (L);
  /* Ensure stack size. */
  luaL_checkstack (L, 2, "cannot check last table key");
  /* Ensure INDEX is absolute. */
  index = lua_absindex (L, index);
  /* Ensure value at INDEX is a table. */
  if (! lua_istable (L, index))
    return luaL_error (L, "attempt to check last key of non-table");
  /* Query key-value pair next to K.  If K is NULL, query first table
     key-value. */
  lua_pushstring (L, k);	/* push nil if K is NULL */
  /* Return value variable. */
  bool retval;
  /* Pop the value pushed above, then push next (first, if K is NULL)
     key-value pair if any and evaluate to true; otherwise push
     nothing and evaluate to false. */
  if (lua_next (L, index))
    {
      lua_pop (L, 2);		/* pop key-value pair */
      /* K is NOT the last key; or table is not empty if K is NULL. */
      retval = false;
      goto end;
    }
  else
    {
      /* K IS the last key; or table is empty if K is NULL. */
      retval = true;
      goto end;
    }
 end:
  /* Release Lua state lock. */
  unlock_lua (L);
  return retval;
}

/* Return true if table at INDEX is empty.  Otherwise, return
   false. */

bool
is_table_empty (lua_State *L, int index)
{
  return is_last_table_key (L, index, NULL);
}


/* functions */
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
  /* Create Lua state L. */
  lua_State *L = create_lua_state ();

  /* Make Lua state L available globally. */
  main_L = L;

  /* path */
  push_lua_path (L);

  lua_getglobal (L, "package");
  lua_getfield (L, -1, "path");
  lua_remove (L, -2);
  lua_concat (L, 2);
  lua_getglobal (L, "package");
  lua_rotate (L, -2, 1);
  lua_setfield (L, -2, "path");
  lua_settop (L, 0);

  /* mininim */
  define_L_mininim (L);

  lua_newuserdata (L, 0);
  luaL_getmetatable (L, L_MININIM);
  lua_setmetatable (L, -2);
  lua_setglobal (L, "MININIM");

  /* Lua 5.0 */
  /* table.getn reference */
  /* lua_pushstring(L, "table"); */
  /* lua_gettable(L, LUA_GLOBALSINDEX); */
  /* lua_pushstring(L, "getn"); */
  /* lua_gettable(L, -2); */
  /* L_set_registry_by_ref (L, &table_getn_ref); */
  /* ------- */
  /* Lua 5.3 */
  /* rawlen reference */
  lua_getglobal (L, "rawlen");
  L_set_registry_by_ref (L, &table_getn_ref);
  /* ------- */

  /* weak registry */
  lua_newtable (L);
  lua_pushstring (L, "__mode");
  lua_pushstring (L, "v");
  lua_rawset (L, -3);
  lua_pushvalue (L, -1);
  lua_setmetatable (L, -2);
  L_set_registry_by_ref (L, &weak_registry_ref);

  /* REPL */
  repl_cond = al_create_cond ();
  debug_cond = al_create_cond ();
  lock_lua (L);
  repl_L = lua_newthread (L);
  L_set_registry_by_ref (L, &repl_thread_ref);
  repl_thread = al_create_thread (repl, repl_L);
  al_start_thread (repl_thread);
  al_wait_cond (repl_cond, get_lua_mutex (L));

  /* load script */
  int e = load_resource ("data/script/mininim.lua",
                         (load_resource_f) load_script, false);

  if (! e)
    {
      if (lua_isfunction (L, -1)) /* run-time error */
	L_call (L, 0, 0);
      else			/* syntax error */
	failure ("%s", lua_tostring(L, -1));
    }
  else
    {
      /* no script found */
      lua_remove (L, 1);
      while (lua_gettop (L))
	{
	  warning ("%s", lua_tostring(L, 1));
	  lua_remove (L, 1);
	}
      exit (EXIT_FAILURE);
    }
  lua_settop (L, 0);
}

void
finalize_script (void)
{
  al_set_thread_should_stop (repl_thread);
  unlock_lua (main_L);

  al_rest (0.1);

  /* These sometimes cause trouble */
  /* al_join_thread (repl_thread, NULL); */
#if ! WINDOWS_EDITION
  al_destroy_thread (repl_thread);
#endif
  al_destroy_cond (repl_cond);
  al_destroy_cond (debug_cond);

  destroy_lua_state (main_L);
}

void *
L_check_type (lua_State *L, int index, const char *tname)
{
  void *ud = luaL_testudata (L, index, tname);
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
    if (repl_prompt_ready) eprintf ("\n");
    eprintf ("%s\n",
	     lua_tostring (L, -1));
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
  /* Lua 5.0 */
  /* int count = lua_getgccount (L); */
  /* int threshold = lua_getgcthreshold (L); */
  /* if (count < threshold) return; */
  /* ------- */

  /* Lua 5.3 */
  /* int count = lua_gc (L, LUA_GCCOUNT, 0); */
  /* int threshold = ? */
  if (! lua_gc (L, LUA_GCISRUNNING, 0)) return;
  /* ------- */

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
L_set_string_var (lua_State *L, int index, char **var)
{
  index = lua_abs_index (L, index);
  const char *value = lua_tostring (L, index);
  if (value) set_string_var (var, value);
}
