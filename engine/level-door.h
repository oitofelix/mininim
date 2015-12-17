/*
  level-door.h -- level door module;

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

#ifndef FREEPOP_LEVEL_DOOR_H
#define FREEPOP_LEVEL_DOOR_H

#include "kernel/video.h"

#define LEVEL_DOOR_MAX_STEP 43

/* bitmaps */
#define VDUNGEON_LEVEL_DOOR_TOP_LEFT "dat/vdungeon/level door/frame upper left.png"
#define VDUNGEON_LEVEL_DOOR_TOP_RIGHT "dat/vdungeon/level door/frame upper right.png"
#define VDUNGEON_LEVEL_DOOR_BOTTOM_LEFT "dat/vdungeon/level door/bottom.png"
#define VDUNGEON_LEVEL_DOOR_BOTTOM_RIGHT "dat/vdungeon/level door/bottom.png"
#define VDUNGEON_LEVEL_DOOR_FLOOR "dat/vdungeon/level door/floor.png"
#define VDUNGEON_LEVEL_DOOR_STAIRS "dat/vdungeon/level door/stairs.png"
#define VDUNGEON_LEVEL_DOOR_FRONT "dat/vdungeon/level door/door bottom.png"

/* sounds */
#define LEVEL_DOOR_OPEN_SAMPLE "dat/digisnd3/level door opening.ogg"
#define LEVEL_DOOR_CLOSE_SAMPLE "dat/digisnd3/res10014.ogg"

/* types */
struct level_door {
  struct pos p;
  int i;
  enum {
    NO_LEVEL_DOOR_ACTION, OPEN_LEVEL_DOOR, CLOSE_LEVEL_DOOR,
  } action;
};

void load_vdungeon_level_door (void);
void unload_level_door (void);
void load_level_door_samples (void);
void unload_level_door_samples (void);
void register_level_door (struct pos *p);
int compare_level_doors (const void *d0, const void *d1);
struct level_door *level_door_at_pos (struct pos *p);
void compute_level_doors (void);
void sample_level_doors (void);
void draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_level_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i);
struct coord *level_door_front_coord_base (struct pos *p, struct coord *c);
struct coord *level_door_front_coord (struct pos *p, struct coord *c, int j, int i);
struct coord *level_door_front_tip_coord (struct pos *p, struct coord *c, int i);
struct coord *level_door_top_left_coord (struct pos *p, struct coord *c);
struct coord *level_door_top_right_coord (struct pos *p, struct coord *c);
struct coord *level_door_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *level_door_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *level_door_stairs_coord (struct pos *p, struct coord *c);
struct coord *level_door_floor_coord (struct pos *p, struct coord *c);

#endif	/* FREEPOP_LEVEL_DOOR_H */
