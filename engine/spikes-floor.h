/*
  spikes-floor.h -- spikes floor module;

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

#ifndef FREEPOP_SPIKES_H
#define FREEPOP_SPIKES_H

#include "kernel/video.h"

/* constants */
#define SPIKES_WAIT SECS_TO_SCYCLES (1)

/* bitmaps */
#define VDUNGEON_SPIKES_FLOOR_LEFT "dat/vdungeon/floor panels/spikes left.png"
#define VDUNGEON_SPIKES_FLOOR_RIGHT "dat/vdungeon/floor panels/spikes right.png"
#define VDUNGEON_SPIKES_LEFT_01 "dat/vdungeon/spikes/frame01 left.png"
#define VDUNGEON_SPIKES_RIGHT_01 "dat/vdungeon/spikes/frame01 right.png"
#define VDUNGEON_SPIKES_LEFT_02 "dat/vdungeon/spikes/frame02 left.png"
#define VDUNGEON_SPIKES_RIGHT_02 "dat/vdungeon/spikes/frame02 right.png"
#define VDUNGEON_SPIKES_LEFT_03 "dat/vdungeon/spikes/frame03 left.png"
#define VDUNGEON_SPIKES_RIGHT_03 "dat/vdungeon/spikes/frame03 right.png"
#define VDUNGEON_SPIKES_LEFT_04 "dat/vdungeon/spikes/frame04 left.png"
#define VDUNGEON_SPIKES_RIGHT_04 "dat/vdungeon/spikes/frame04 right.png"
#define VDUNGEON_SPIKES_LEFT_05 "dat/vdungeon/spikes/frame05 left.png"
#define VDUNGEON_SPIKES_RIGHT_05 "dat/vdungeon/spikes/frame05 right.png"
#define VDUNGEON_SPIKES_01_FG "dat/vdungeon/spikes/frame01.png"
#define VDUNGEON_SPIKES_02_FG "dat/vdungeon/spikes/frame02.png"
#define VDUNGEON_SPIKES_03_FG "dat/vdungeon/spikes/frame03.png"
#define VDUNGEON_SPIKES_04_FG "dat/vdungeon/spikes/frame04.png"
#define VDUNGEON_SPIKES_05_FG "dat/vdungeon/spikes/frame05.png"

/* sounds */
#define SPIKES_SOUND "dat/digisnd2/spikes.ogg"

/* types */
struct spikes_floor {
  struct pos p;
  int i, wait, state;
};

void load_vdungeon_spikes_floor (void);
void unload_spikes_floor (void);
void load_spikes_floor_sounds (void);
void unload_spikes_floor_sounds (void);
void register_spikes_floor (struct pos p);
struct spikes_floor * spikes_floor_at_pos (struct pos p);
void break_spikes_floor (struct pos p);
void compute_spikes_floors (void);
bool should_spikes_raise_for_pos (struct pos p, struct pos pk);
bool should_spikes_raise (struct pos p);
void draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_floor_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes (ALLEGRO_BITMAP *bitmap, struct pos p,
                  struct spikes_floor *s);
void draw_spikes_left (ALLEGRO_BITMAP *bitmap, struct pos p,
                       struct spikes_floor *s);
void draw_spikes_right (ALLEGRO_BITMAP *bitmap, struct pos p,
                        struct spikes_floor *s);
void draw_spikes_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_left_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_right_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_01_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord spikes_left_01_coord (struct pos p);
struct coord spikes_right_01_coord (struct pos p);
struct coord spikes_01_fg_coord (struct pos p);
void draw_spikes_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_left_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_right_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_02_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord spikes_left_02_coord (struct pos p);
struct coord spikes_right_02_coord (struct pos p);
struct coord spikes_02_fg_coord (struct pos p);
void draw_spikes_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_left_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_right_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_03_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord spikes_left_03_coord (struct pos p);
struct coord spikes_right_03_coord (struct pos p);
struct coord spikes_03_fg_coord (struct pos p);
void draw_spikes_04 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_left_04 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_right_04 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_04_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord spikes_left_04_coord (struct pos p);
struct coord spikes_right_04_coord (struct pos p);
struct coord spikes_04_fg_coord (struct pos p);
void draw_spikes_05 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_left_05 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_right_05 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_spikes_05_fg (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord spikes_left_05_coord (struct pos p);
struct coord spikes_right_05_coord (struct pos p);
struct coord spikes_05_fg_coord (struct pos p);

#endif	/* FREEPOP_SPIKES_H */
