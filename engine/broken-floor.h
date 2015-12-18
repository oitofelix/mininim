/*
  broken-floor.h -- broken floor module;

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

#ifndef FREEPOP_BROKEN_FLOOR_H
#define FREEPOP_BROKEN_FLOOR_H

#include "kernel/audio.h"
#include "physics.h"

/* dungeon vga */
#define DV_BROKEN_FLOOR_LEFT "data/broken-floor/dv-left.png"
#define DV_BROKEN_FLOOR_RIGHT "data/broken-floor/dv-right.png"
#define DV_BROKEN_FLOOR_FRONT "data/broken-floor/dv-front.png"

extern ALLEGRO_BITMAP *dv_broken_floor_left, *dv_broken_floor_right,
  *dv_broken_floor_front;

/* palace vga */
#define PV_BROKEN_FLOOR_LEFT "data/broken-floor/pv-left.png"
#define PV_BROKEN_FLOOR_RIGHT "data/broken-floor/pv-right.png"
#define PV_BROKEN_FLOOR_FRONT "data/broken-floor/pv-front.png"

extern ALLEGRO_BITMAP *pv_broken_floor_left, *pv_broken_floor_right,
  *pv_broken_floor_front;

void load_broken_floor (void);
void unload_broken_floor (void);
void draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm);
void draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm);
void draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm);
void draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm);
struct coord *broken_floor_right_coord (struct pos *p, struct coord *c);
struct coord *broken_floor_front_coord (struct pos *p, struct coord *c);
ALLEGRO_BITMAP * create_broken_floor_bitmap (enum em em, enum vm);

#endif	/* FREEPOP_BROKEN_FLOOR_H */
