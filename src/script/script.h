/*
  script.h -- script module;

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

#ifndef MININIM_SCRIPT_H
#define MININIM_SCRIPT_H

#define DEFUN(name) int name (lua_State *L)
#define lua_abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
                             lua_gettop(L) + (i) + 1)

/* Structure whose pointer is meant to be stored in the extra space of
   Lua states.  It's meant to hold data associated with their
   respective Lua states.  See functions lua_getextraspace,
   alloc_lua_extra_space, get_lua_extra_space and
   free_lua_extra_space.*/

struct lua_extra_space
{
  ALLEGRO_MUTEX *mutex;
};


/* Lua state */
lua_State *create_lua_state ();
void destroy_lua_state (lua_State *L);


/* Lua extra space */
struct lua_extra_space *create_lua_extra_space (lua_State *L);
void destroy_lua_extra_space (lua_State *L);
struct lua_extra_space *get_lua_extra_space (lua_State *L);


/* Lua recursive mutex */
ALLEGRO_MUTEX *create_lua_mutex (struct lua_extra_space *les_ptr);
void destroy_lua_mutex (struct lua_extra_space *les_ptr);
ALLEGRO_MUTEX *get_lua_mutex (lua_State *L);
void lock_lua (lua_State *L);
void unlock_lua (lua_State *L);


/* Lua path */
bool add_to_lua_path_buffer (void_luaL_Buffer *data,
			     const char *dir);
int push_lua_path (lua_State *L);


/* Table facilities */
int remove_table_field (lua_State *L, int index, const char *k);
bool is_last_table_key (lua_State *L, int index, const char *k);
bool is_table_empty (lua_State *L, int index);


/* functions */
int remove_table_field (lua_State *L, int index, const char *k);
bool is_last_table_key (lua_State *L, int index, const char *k);
bool is_table_empty (lua_State *L, int index);

void init_script (void);
void finalize_script (void);

int lua_getn (lua_State *L, int index);
lua_Number L_rawgeti_tonumber (lua_State *L, int index, int n);

void *L_check_type (lua_State *L, int index, const char *tname);
bool L_call (lua_State *L, int nargs, int nresults);
bool L_run_hook (lua_State *L);

void L_set_registry_by_ptr (lua_State *L, void *p);
void L_get_registry_by_ptr (lua_State *L, void *p);
int L_set_registry_by_ref (lua_State *L, int *r);
void L_get_registry_by_ref (lua_State *L, int r);
void L_push_interface (lua_State *L, const char *tname);

void L_set_weak_registry_by_ptr (lua_State *L, void *p);
void L_get_weak_registry_by_ptr (lua_State *L, void *p);

void L_gc (lua_State *L);

int L_error_expected_got (lua_State *L, int index,
                          const char *expected_tname);

void L_set_string_var (lua_State *L, int index, char **var);

/* variables */
extern lua_State *main_L;

#endif	/* MININIM_SCRIPT_H */
