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

#ifndef MININIM_SKELETON_FLOOR_H
#define MININIM_SKELETON_FLOOR_H

/* dungeon cga */
#define DC_SKELETON_FLOOR_LEFT "data/skeleton-floor/dc-left.png"
#define DC_SKELETON_FLOOR_RIGHT "data/skeleton-floor/dc-right.png"

/* palace cga */
#define PC_SKELETON_FLOOR_LEFT "data/skeleton-floor/pc-left.png"
#define PC_SKELETON_FLOOR_RIGHT "data/skeleton-floor/pc-right.png"

/* dungeon ega */
#define DE_SKELETON_FLOOR_LEFT "data/skeleton-floor/de-left.png"
#define DE_SKELETON_FLOOR_RIGHT "data/skeleton-floor/de-right.png"

/* palace ega */
#define PE_SKELETON_FLOOR_LEFT "data/skeleton-floor/pe-left.png"
#define PE_SKELETON_FLOOR_RIGHT "data/skeleton-floor/pe-right.png"

/* dungeon vga */
#define DV_SKELETON_FLOOR_LEFT "data/skeleton-floor/dv-left.png"
#define DV_SKELETON_FLOOR_RIGHT "data/skeleton-floor/dv-right.png"

/* palace vga */
#define PV_SKELETON_FLOOR_LEFT "data/skeleton-floor/pv-left.png"
#define PV_SKELETON_FLOOR_RIGHT "data/skeleton-floor/pv-right.png"

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

#endif	/* MININIM_SKELETON_FLOOR_H */
