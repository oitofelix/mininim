/*
  script.h -- script module;

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

#ifndef MININIM_SCRIPT_H
#define MININIM_SCRIPT_H

/* functions */
void init_script (void);
void finalize_script (void);

void *L_check_type (lua_State *L, int index, const char *tname);
bool L_call (lua_State *L, int nargs, int nresults, int errfunc);
bool L_run_hook (lua_State *L);
int L_set_registry (lua_State *L, int *r);
void L_get_registry (lua_State *L, int r);
void L_push_interface (lua_State *L, const char *tname);

/* variables */
extern lua_State *main_L;

#endif	/* MININIM_SCRIPT_H */
