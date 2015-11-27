/*
  room.h -- room module;

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

#ifndef FREEPOP_ROOM_H
#define FREEPOP_ROOM_H

#include "prince.h"

#define VDUNGEON_BRICKS_01 "dat/vdungeon/background/bricks01.png"
#define VDUNGEON_BRICKS_02 "dat/vdungeon/background/bricks02.png"
#define VDUNGEON_BRICKS_03 "dat/vdungeon/background/bricks03.png"
#define VDUNGEON_BRICKS_04 "dat/vdungeon/background/bricks04.png"
#define VDUNGEON_TORCH "dat/vdungeon/background/torch.png"
#define VDUNGEON_WINDOW "dat/vdungeon/background/window.png"

extern ALLEGRO_BITMAP *room_bg;
extern bool redraw_room;

void load_room (void);
void unload_room (void);
struct coord nbitmap_coord (struct coord c, int w, int h);
void draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                     struct coord c, int flags);
void draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to, float sx, float sy,
                     float sw, float sh, struct coord c, int flags);
void draw_room (int room);
void draw_room_bg (void);
void draw_con (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_confg (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_con_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_con_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_no_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_no_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord bricks_coord_12 (struct pos p);
struct coord bricks_coord_34 (struct pos p);
void draw_torch (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord torch_coord (struct pos p);
void draw_window (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord window_coord (struct pos p);
void draw_room_anim_fg (struct anim a);
void draw_room_fg (struct anim a, struct pos p);

#endif	/* FREEPOP_ROOM_H */
