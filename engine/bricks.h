/*
  bricks.h -- bricks module;

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

#ifndef FREEPOP_BRICKS_H
#define FREEPOP_BRICKS_H

/* dungeon vga */
#define DV_BRICKS_01 "data/bricks/dv-01.png"
#define DV_BRICKS_02 "data/bricks/dv-02.png"
#define DV_BRICKS_03 "data/bricks/dv-03.png"
#define DV_BRICKS_04 "data/bricks/dv-04.png"

/* palace vga */
#define PV_BRICKS_01 "data/bricks/pv-01.png"
#define PV_BRICKS_02 "data/bricks/pv-02.png"
#define PV_BRICKS_03 "data/bricks/pv-03.png"
#define PV_BRICKS_04 "data/bricks/pv-04.png"

void load_bricks (void);
void unload_bricks (void);
void draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
struct coord *d_bricks_coord_01 (struct pos *p, struct coord *c);
struct coord *p_bricks_coord_01 (struct pos *p, struct coord *c);
struct coord *d_bricks_coord_02 (struct pos *p, struct coord *c);
struct coord *p_bricks_coord_02 (struct pos *p, struct coord *c);
struct coord *d_bricks_coord_03 (struct pos *p, struct coord *c);
struct coord *p_bricks_coord_03 (struct pos *p, struct coord *c);
struct coord *d_bricks_coord_04 (struct pos *p, struct coord *c);
struct coord *p_bricks_coord_04 (struct pos *p, struct coord *c);

#endif	/* FREEPOP_BRICKS_H */
