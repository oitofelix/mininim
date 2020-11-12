/*
  L_mininim.debugger.h -- mininim.debugger script module;

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

#ifndef MININIM_L_MININIM_DEBUGGER_H
#define MININIM_L_MININIM_DEBUGGER_H

#define L_MININIM_DEBUGGER L_MININIM ".debugger"

/* functions */
void define_L_mininim_debugger (lua_State *L);
void L_debug_main_thread (void);
void debugger_hook (lua_State *L, lua_Debug *ar);
char *ar_tostring (lua_Debug *ar);
char *get_cstack_report (lua_State *L, const char *fmt);
char *L_typename (lua_State *L, int index);

DEFUN (L_TRACEBACK);

/* variables */
bool L_debugging;
ALLEGRO_COND *debug_cond;

#endif	/* MININIM_L_MININIM_DEBUGGER_H */
