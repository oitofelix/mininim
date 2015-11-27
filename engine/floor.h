/*
  floor.h -- floor module;

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

#ifndef FREEPOP_FLOOR_H
#define FREEPOP_FLOOR_H

#include "kernel/audio.h"
#include "physics.h"

/* bitmaps */
#define VDUNGEON_NORMAL_FLOOR_LEFT "dat/vdungeon/floor panels/normal left.png"
#define VDUNGEON_NORMAL_FLOOR_RIGHT "dat/vdungeon/floor panels/normal right.png"
#define VDUNGEON_NORMAL_FLOOR_BASE "dat/vdungeon/floor panels/normal base.png"
#define VDUNGEON_BROKEN_FLOOR_LEFT "dat/vdungeon/floor panels/broken left.png"
#define VDUNGEON_BROKEN_FLOOR_RIGHT "dat/vdungeon/floor panels/broken right.png"
#define VDUNGEON_BROKEN_FLOOR_FRONT "dat/vdungeon/floor panels/broken left02.png"
#define VDUNGEON_SKELETON_FLOOR_LEFT "dat/vdungeon/floor panels/skeleton left.png"
#define VDUNGEON_SKELETON_FLOOR_RIGHT "dat/vdungeon/floor panels/skeleton right.png"
#define VDUNGEON_FLOOR_CORNER_01 "dat/vdungeon/floor panels/normal_closer01.png"
#define VDUNGEON_FLOOR_CORNER_02 "dat/vdungeon/floor panels/normal_closer02.png"
#define VDUNGEON_FLOOR_CORNER_03 "dat/vdungeon/floor panels/normal_closer03.png"

extern ALLEGRO_BITMAP *normal_floor_left, *normal_floor_right, *normal_floor_base,
  *broken_floor_left, *broken_floor_right, *broken_floor_front;

void load_vdungeon_floor (void);
void unload_floor (void);
void draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_skeleton_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_skeleton_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_skeleton_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_skeleton_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord floor_base_coord (struct pos pos);
struct coord floor_left_coord (struct pos pos);
struct coord floor_right_coord (struct pos pos);
struct coord broken_floor_right_coord (struct pos p);
struct coord broken_floor_front_coord (struct pos pos);
struct coord skeleton_floor_left_coord (struct pos p);
struct coord skeleton_floor_right_coord (struct pos p);
struct coord floor_corner_01_coord (struct pos p);
struct coord floor_corner_02_coord (struct pos p);
struct coord floor_corner_03_coord (struct pos p);

#endif	/* FREEPOP_FLOOR_H */
