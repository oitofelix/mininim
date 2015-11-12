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

/* bitmaps */
#define VDUNGEON_FLOOR_LOOSE_LEFT_01 "dat/vdungeon/floor panels/loose left01.png"
#define VDUNGEON_FLOOR_LOOSE_RIGHT_01 "dat/vdungeon/floor panels/loose right01.png"
#define VDUNGEON_FLOOR_LOOSE_BASE_01 "dat/vdungeon/floor panels/loose base01.png"
#define VDUNGEON_FLOOR_LOOSE_LEFT_02 "dat/vdungeon/floor panels/loose left02.png"
#define VDUNGEON_FLOOR_LOOSE_RIGHT_02 "dat/vdungeon/floor panels/loose right02.png"
#define VDUNGEON_FLOOR_LOOSE_BASE_02 "dat/vdungeon/floor panels/loose base02.png"

/* sounds */
#define LOOSE_FLOOR_01 "dat/digisnd1/tile moving 1.ogg"
#define LOOSE_FLOOR_02 "dat/digisnd1/tile moving 2.ogg"
#define LOOSE_FLOOR_03 "dat/digisnd1/door tile pressed 1.ogg"
#define BROKEN_FLOOR_SOUND "dat/digisnd1/tile crashing into the floor.ogg"

/* types */
struct loose_floor {
  struct pos p;
  int i;
  struct anim a;
  int resist;
  void (*draw) (struct loose_floor *f);
};

/* functions */
void load_vdungeon_loose_floor (void);
void unload_loose_floor (void);
ALLEGRO_BITMAP *create_floor_loose_02_bitmap (void);
void draw_shake_floor (void);
void release_loose_floor (struct pos p);
ALLEGRO_SAMPLE *loose_floor_sample (void);
void draw_release_loose_floor (void);
void draw_floor_fall (void);
void draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord floor_loose_left_coord (struct pos p);
struct coord floor_loose_right_coord (struct pos p);

#endif	/* FREEPOP_LOOSE_FLOOR_H */
