/*
  big-pillar.h -- big pillar module;

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

#ifndef FREEPOP_BIG_PILLAR_H
#define FREEPOP_BIG_PILLAR_H

#include "physics.h"

#define VDUNGEON_BIG_PILLAR_BOTTOM_LEFT "dat/vdungeon/bigpillar/big pillar left lower.png"
#define VDUNGEON_BIG_PILLAR_BOTTOM_RIGHT "dat/vdungeon/bigpillar/big pillar right lower.png"
#define VDUNGEON_BIG_PILLAR_TOP_LEFT "dat/vdungeon/bigpillar/big pillar left upper.png"
#define VDUNGEON_BIG_PILLAR_TOP_RIGHT "dat/vdungeon/bigpillar/big pillar right upper.png"
#define VDUNGEON_BIG_PILLAR_TOP_RIGHT_TOP "dat/vdungeon/bigpillar/big pillar right upper top.png"
#define VDUNGEON_BIG_PILLAR_BOTTOM_FG "dat/vdungeon/bigpillar/big pillar lower02.png"


extern ALLEGRO_BITMAP *big_pillar_bottom_left, *big_pillar_bottom_right,
  *big_pillar_top_left, *big_pillar_top_right, *big_pillar_top_right_top,
  *big_pillar_bottom_fg;

void load_vdungeon_big_pillar (void);
void unload_big_pillar (void);
void draw_big_pillar_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_bottom_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_bottom_right (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_top (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_big_pillar_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p);
struct coord *big_pillar_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_top_left_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_top_right_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_top_right_top_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_bottom_fg_coord (struct pos *p, struct coord *c);

#endif	/* FREEPOP_BIG_PILLAR_H */
