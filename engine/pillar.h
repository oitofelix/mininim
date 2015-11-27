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

#define VDUNGEON_PILLAR_LEFT "dat/vdungeon/pillar/pillar left.png"
#define VDUNGEON_PILLAR_RIGHT "dat/vdungeon/pillar/pillar right main.png"
#define VDUNGEON_PILLAR_TOP "dat/vdungeon/pillar/pillar right top.png"
#define VDUNGEON_PILLAR "dat/vdungeon/pillar/pillar.png"

extern ALLEGRO_BITMAP *pillar_left, *pillar_right, *pillar_top, *pillar;

void load_vdungeon_pillar (void);
void unload_pillar (void);
void draw_pillar (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_pillar_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pillar_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pillar_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pillar_fg (ALLEGRO_BITMAP *bitmap, struct pos pos);
struct coord pillar_coord (struct pos pos);
struct coord pillar_left_coord (struct pos pos);
struct coord pillar_right_coord (struct pos pos);
struct coord pillar_top_coord (struct pos pos);

#endif	/* FREEPOP_PILLAR_H */
