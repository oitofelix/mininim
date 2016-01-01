/*
  window.h -- window module;

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

#ifndef MININIM_WINDOW_H
#define MININIM_WINDOW_H

/* dungeon cga */
#define DC_WINDOW "data/window/dc.png"

/* palace cga */
#define PC_WINDOW "data/window/pc.png"

/* dungeon ega */
#define DE_WINDOW "data/window/de.png"

/* palace ega */
#define PE_WINDOW "data/window/pe.png"

/* dungeon vga */
#define DV_WINDOW "data/window/dv.png"

/* palace vga */
#define PV_WINDOW "data/window/pv.png"

void load_window (void);
void unload_window (void);
void draw_window (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm);
struct coord *d_window_coord (struct pos *p, struct coord *c);
struct coord *p_window_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_WINDOW_H */
