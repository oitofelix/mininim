/*
  mouse.h -- mouse module;

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

#ifndef MININIM_MOUSE_H
#define MININIM_MOUSE_H

#define MOUSE_NORMAL_00 "data/mouse/normal/00.png"
#define MOUSE_RUN_00 "data/mouse/run/00.png"
#define MOUSE_RUN_01 "data/mouse/run/01.png"

/* functions */
void load_mouse (void);
void unload_mouse (void);
struct actor *create_mouse (struct actor *m0, struct actor *m1,
                           struct pos *p, enum dir dir);
void draw_mouse_frame (ALLEGRO_BITMAP *bitmap, struct actor *m);
void mouse_normal (struct actor *mouse);
void mouse_run (struct actor *mouse);

/* variables */
extern ALLEGRO_BITMAP *mouse_normal_00, *mouse_run_00, *mouse_run_01;

#endif	/* MININIM_MOUSE_H */
