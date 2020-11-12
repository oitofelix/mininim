/*
  pointer.h -- pointer module;

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

#ifndef MININIM_POINTER_H
#define MININIM_POINTER_H

/* variables */
extern struct pos mouse_pos;
extern struct pos selection_pos, last_selection_pos;

/* functions */
void init_mouse (void);
void finalize_mouse (void);
struct mouse_coord *get_mouse_coord (struct mr *mr, struct mouse_coord *m);
struct pos *get_mouse_pos (struct mr *mr, struct pos *p);
void set_mouse_coord (struct mr *mr, struct mouse_coord *m);
void set_mouse_pos (struct mr *mr, struct pos *p);
void set_mouse_room (struct mr *mr, int room);

#endif	/* MININIM_POINTER_H */
