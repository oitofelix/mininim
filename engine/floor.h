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
#define VDUNGEON_FLOOR_NORMAL_LEFT "dat/vdungeon/floor panels/normal left.png"
#define VDUNGEON_FLOOR_NORMAL_RIGHT "dat/vdungeon/floor panels/normal right.png"
#define VDUNGEON_FLOOR_NORMAL_BASE "dat/vdungeon/floor panels/normal base.png"
#define VDUNGEON_FLOOR_BROKEN_LEFT "dat/vdungeon/floor panels/broken left.png"
#define VDUNGEON_FLOOR_BROKEN_RIGHT "dat/vdungeon/floor panels/broken right.png"
#define VDUNGEON_FLOOR_BROKEN_FRONT "dat/vdungeon/floor panels/broken left02.png"
#define VDUNGEON_FLOOR_LOOSE_LEFT_01 "dat/vdungeon/floor panels/loose left01.png"
#define VDUNGEON_FLOOR_LOOSE_RIGHT_01 "dat/vdungeon/floor panels/loose right01.png"
#define VDUNGEON_FLOOR_LOOSE_BASE_01 "dat/vdungeon/floor panels/loose base01.png"
#define VDUNGEON_FLOOR_LOOSE_LEFT_02 "dat/vdungeon/floor panels/loose left02.png"
#define VDUNGEON_FLOOR_LOOSE_RIGHT_02 "dat/vdungeon/floor panels/loose right02.png"
#define VDUNGEON_FLOOR_LOOSE_BASE_02 "dat/vdungeon/floor panels/loose base02.png"
#define VDUNGEON_FLOOR_CORNER_01 "dat/vdungeon/floor panels/normal_closer01.png"
#define VDUNGEON_FLOOR_CORNER_02 "dat/vdungeon/floor panels/normal_closer02.png"
#define VDUNGEON_FLOOR_CORNER_03 "dat/vdungeon/floor panels/normal_closer03.png"
#define VDUNGEON_FLOOR_SPIKES_LEFT "dat/vdungeon/floor panels/spikes left.png"
#define VDUNGEON_FLOOR_SPIKES_RIGHT "dat/vdungeon/floor panels/spikes right.png"

/* sounds */
#define LOOSE_FLOOR_01 "dat/digisnd1/tile moving 1.ogg"
#define LOOSE_FLOOR_02 "dat/digisnd1/tile moving 2.ogg"
#define LOOSE_FLOOR_03 "dat/digisnd1/door tile pressed 1.ogg"
#define BROKEN_FLOOR_SOUND "dat/digisnd1/tile crashing into the floor.ogg"

struct loose_floor {
  struct pos p;
  int i;
  struct anim a;
  int resist;
  void (*draw) (struct loose_floor *f);
};

extern ALLEGRO_BITMAP *floor_normal_left, *floor_normal_right, *floor_normal_base,
  *floor_broken_left, *floor_broken_right, *floor_broken_front;

ALLEGRO_SAMPLE * loose_floor_sample (void);
void load_vdungeon_floor (void);
void unload_floor (void);
ALLEGRO_BITMAP *create_floor_loose_02_bitmap (void);
void draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_broken_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_broken_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos pos);
void draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos pos);
struct coord floor_base_coord (struct pos pos);
struct coord floor_left_coord (struct pos pos);
struct coord floor_right_coord (struct pos pos);
struct coord broken_floor_front_coord (struct pos pos);
void release_loose_floor (struct pos p);
void draw_shake_floor (void);
void draw_release_loose_floor (void);
void draw_floor_fall (void);
void draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord floor_loose_base_coord (struct pos p);
struct coord floor_loose_left_coord (struct pos p);
struct coord floor_loose_right_coord (struct pos p);
void draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord floor_corner_01_coord (struct pos p);
struct coord floor_corner_02_coord (struct pos p);
struct coord floor_corner_03_coord (struct pos p);

#endif	/* FREEPOP_FLOOR_H */
