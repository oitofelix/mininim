/*
  repl.h -- Lua REPL module;

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

#ifndef MININIM_XLUA_H
#define MININIM_XLUA_H

/* functions */
void *repl (ALLEGRO_THREAD *thread, void *repl_L);
void repl_update_prompt (void);

/* variables */
extern ALLEGRO_MUTEX *repl_mutex;
extern ALLEGRO_THREAD *repl_thread;
extern ALLEGRO_COND *repl_cond;
extern lua_State *repl_L;
extern int repl_thread_ref;
extern bool repl_prompt_ready;

#endif	/* MININIM_XLUA_H */
