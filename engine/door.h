/*
  door.h -- door module;

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

#ifndef FREEPOP_DOOR_H
#define FREEPOP_DOOR_H

#include "kernel/video.h"

#define DOOR_MAX_STEP 47
#define DOOR_WAIT SECS_TO_SCYCLES (5)

#define DOOR_CLIMB_LIMIT 40
#define DOOR_WAIT_LOOK 4

/* bitmaps */
#define VDUNGEON_DOOR_LEFT "dat/vdungeon/door/door frame left.png"
#define VDUNGEON_DOOR_RIGHT "dat/vdungeon/door/door frame right.png"
#define VDUNGEON_DOOR_POLE "dat/vdungeon/door/door frame left pole.png"
#define VDUNGEON_DOOR_TOP "dat/vdungeon/door/door frame right top.png"
#define VDUNGEON_DOOR_GRID "dat/vdungeon/door/res00252.png"
#define VDUNGEON_DOOR_GRID_TIP "dat/vdungeon/door/res00251.png"
#define VDUNGEON_DOOR_GRID_TOP "dat/vdungeon/door/res00260.png"

/* sounds */
#define DOOR_OPEN_SOUND "dat/digisnd1/door gate closing slow 1.ogg"
#define DOOR_CLOSE_SOUND "dat/digisnd1/door gate opening 1.ogg"
#define DOOR_END_SOUND "dat/digisnd1/drinking potion.ogg"
#define DOOR_ABRUPTLY_CLOSE_SOUND "dat/digisnd1/door gate closing fast.ogg"

/* types */
struct door {
  struct pos p;
  int i;
  enum {
    NO_DOOR_ACTION, OPEN_DOOR, CLOSE_DOOR, ABRUPTLY_CLOSE_DOOR,
  } action;
  int wait;
  bool noise;
};

void load_vdungeon_door (void);
void unload_door (void);
void load_door_sounds (void);
void unload_door_sounds (void);
void register_door (struct pos p);
struct door *door_at_pos (struct pos p);
void open_door (int e);
void close_door (int e);
void compute_doors (void);
int door_grid_tip_y (struct pos p);
void draw_door (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_door_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_door_frame (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_door_frame_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_door_frame_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos p,
                   struct anim a);
void draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos p, int i);
struct coord door_grid_coord_base (struct pos p);
struct coord door_grid_coord (struct pos p, int j, int i);
struct coord door_grid_tip_coord (struct pos p, int i);
struct coord door_grid_top_coord (struct pos p);
struct coord door_pole_coord (struct pos p);
struct coord door_left_coord (struct pos p);
struct coord door_right_coord (struct pos p);
struct coord door_top_coord (struct pos p);

#endif	/* FREEPOP_DOOR_H */
