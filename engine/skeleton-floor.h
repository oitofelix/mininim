/*
  skeleton-floor.h -- skeleton floor module;

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

#ifndef FREEPOP_SKELETON_FLOOR_H
#define FREEPOP_SKELETON_FLOOR_H

#include "kernel/audio.h"
#include "physics.h"

/* dungeon vga */
#define DV_SKELETON_FLOOR_LEFT "dat/vdungeon/floor panels/skeleton left.png"
#define DV_SKELETON_FLOOR_RIGHT "dat/vdungeon/floor panels/skeleton right.png"

extern ALLEGRO_BITMAP *dv_skeleton_floor_left, *dv_skeleton_floor_right;

/* palace vga */
#define PV_SKELETON_FLOOR_LEFT "dat/vpalace/floor panels/skeleton left.png"
#define PV_SKELETON_FLOOR_RIGHT "dat/vpalace/floor panels/skeleton right.png"

extern ALLEGRO_BITMAP *pv_skeleton_floor_left, *pv_skeleton_floor_right;

void load_skeleton_floor (void);
void unload_skeleton_floor (void);
void draw_skeleton_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm);
void draw_skeleton_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm);
void draw_skeleton_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm);
struct coord *skeleton_floor_left_coord (struct pos *p, struct coord *c);
struct coord *skeleton_floor_right_coord (struct pos *p, struct coord *c);

#endif	/* FREEPOP_SKELETON_FLOOR_H */
