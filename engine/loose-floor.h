/*
  loose-floor.h -- loose floor module;

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

#ifndef FREEPOP_LOOSE_FLOOR_H
#define FREEPOP_LOOSE_FLOOR_H

#include "kernel/audio.h"

/* constants */
#define LOOSE_FLOOR_RESISTENCE 0

/* bitmaps */
#define VDUNGEON_LOOSE_FLOOR_LEFT_01 "dat/vdungeon/floor panels/loose left01.png"
#define VDUNGEON_LOOSE_FLOOR_RIGHT_01 "dat/vdungeon/floor panels/loose right01.png"
#define VDUNGEON_LOOSE_FLOOR_BASE_01 "dat/vdungeon/floor panels/loose base01.png"
#define VDUNGEON_LOOSE_FLOOR_LEFT_02 "dat/vdungeon/floor panels/loose left02.png"
#define VDUNGEON_LOOSE_FLOOR_RIGHT_02 "dat/vdungeon/floor panels/loose right02.png"
#define VDUNGEON_LOOSE_FLOOR_BASE_02 "dat/vdungeon/floor panels/loose base02.png"

/* sounds */
#define LOOSE_FLOOR_01_SOUND "dat/digisnd1/tile moving 1.ogg"
#define LOOSE_FLOOR_02_SOUND "dat/digisnd1/tile moving 2.ogg"
#define LOOSE_FLOOR_03_SOUND "dat/digisnd1/door tile pressed 1.ogg"
#define BROKEN_FLOOR_SOUND "dat/digisnd1/tile crashing into the floor.ogg"

/* types */
struct loose_floor {
  struct pos p;
  int i;
  int resist;
  int state;

  enum {
    NO_LOOSE_FLOOR_ACTION,
    SHAKE_LOOSE_FLOOR,
    RELEASE_LOOSE_FLOOR,
    FALL_LOOSE_FLOOR,
  } action;

  struct anim a;
};

/* functions */
void load_vdungeon_loose_floor (void);
void unload_loose_floor (void);
void load_loose_floor_sounds (void);
void unload_loose_floor_sounds (void);
ALLEGRO_BITMAP *create_loose_floor_02_bitmap (void);
ALLEGRO_BITMAP * create_broken_floor_bitmap (void);
void register_loose_floor (struct pos p);
struct loose_floor * loose_floor_at_pos (struct pos p);
void remove_loose_floor (struct loose_floor *l);
void release_loose_floor (struct pos p);
void compute_loose_floors (void);
void compute_loose_floor_shake (struct loose_floor *l);
void compute_loose_floor_release (struct loose_floor *l);
void compute_loose_floor_fall (struct loose_floor *l);
void shake_loose_floor_row (struct pos p);
ALLEGRO_SAMPLE * loose_floor_sample (void);
void draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_00 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_00_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_00_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_00_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_01_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02_right (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord loose_floor_left_coord (struct pos p);
struct coord loose_floor_right_coord (struct pos p);

#endif	/* FREEPOP_LOOSE_FLOOR_H */
