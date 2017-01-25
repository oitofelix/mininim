/*
  editor.h -- editor module;

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

#ifndef MININIM_EDITOR_H
#define MININIM_EDITOR_H

/* variables */
extern enum edit edit;
extern enum edit last_edit;
extern uint64_t editor_register;

/* functions */
bool can_edit (void);
void editor (void);
void enter_editor (void);
void exit_editor (int priority);
void editor_msg (char *m, uint64_t cycles);
void ui_place_kid (struct anim *k, struct pos *p);
void ui_place_guard (struct anim *g, struct pos *p);
void editor_link (int room0, int room1, enum dir dir);
void editor_mirror_link (int room, enum dir dir0, enum dir dir1);

#endif	/* MININIM_EDITOR_H */
