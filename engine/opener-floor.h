/*
  opener-floor.h -- opener floor module;

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

#ifndef FREEPOP_OPENER_FLOOR_H
#define FREEPOP_OPENER_FLOOR_H

#include "pos.h"

/* bitmaps */
#define VDUNGEON_UNPRESSED_OPENER_FLOOR_LEFT "dat/vdungeon/floor panels/opener left unpressed.png"
#define VDUNGEON_UNPRESSED_OPENER_FLOOR_BASE "dat/vdungeon/floor panels/opener base unpressed.png"

/* sounds */
#define OPENER_FLOOR_SOUND "dat/digisnd1/door tile pressed 1.ogg"

/* types */
struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  bool broken;
};

void load_vdungeon_opener_floor (void);
void unload_opener_floor (void);
void load_opener_floor_sounds (void);
void unload_opener_floor_sounds (void);
void register_opener_floor (struct pos p);
struct opener_floor * opener_floor_at_pos (struct pos p);
void press_opener_floor (struct pos p);
void break_opener_floor (struct pos p);
void unpress_opener_floors (void);
void compute_opener_floors (void);
void draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord unpressed_opener_floor_left_coord (struct pos p);
struct coord unpressed_opener_floor_right_coord (struct pos p);

#endif	/* FREEPOP_OPENER_FLOOR_H */
