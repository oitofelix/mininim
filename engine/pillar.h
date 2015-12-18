/*
  pillar.h -- pillar module;

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

#ifndef FREEPOP_PILLAR_H
#define FREEPOP_PILLAR_H

#include "physics.h"

/* dungeon vga */
#define DV_PILLAR_LEFT "data/pillar/dv-left.png"
#define DV_PILLAR_RIGHT "data/pillar/dv-right.png"
#define DV_PILLAR_TOP "data/pillar/dv-top.png"
#define DV_PILLAR_FG "data/pillar/dv-fg.png"

/* palace vga */
#define PV_PILLAR_LEFT "data/pillar/pv-left.png"
#define PV_PILLAR_RIGHT "data/pillar/pv-right.png"
#define PV_PILLAR_TOP "data/pillar/pv-top.png"
#define PV_PILLAR_FG "data/pillar/pv-fg.png"

void load_pillar (void);
void unload_pillar (void);
void draw_pillar (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm);
void draw_pillar_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm);
void draw_pillar_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_pillar_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
struct coord *pillar_coord (struct pos *p, struct coord *c);
struct coord *pillar_left_coord (struct pos *p, struct coord *c);
struct coord *pillar_right_coord (struct pos *p, struct coord *c);
struct coord *pillar_top_coord (struct pos *p, struct coord *c);

#endif	/* FREEPOP_PILLAR_H */
