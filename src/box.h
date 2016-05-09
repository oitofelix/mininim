/*
  box.h -- box module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_BOX_H
#define MININIM_BOX_H

/* vga */
#define V_BOX_00 "data/box/00.png"
#define V_BOX_01 "data/box/01.png"
#define V_BOX_02 "data/box/02.png"

void load_box (void);
void unload_box (void);
void draw_box (ALLEGRO_BITMAP* bitmap, struct pos *p, enum vm vm);
struct coord *box_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_BOX_H */
