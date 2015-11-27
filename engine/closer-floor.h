/*
  closer-floor.h -- closer floor module;

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

#ifndef FREEPOP_CLOSER_FLOOR_H
#define FREEPOP_CLOSER_FLOOR_H

#include "pos.h"

/* bitmaps */
#define VDUNGEON_UNPRESSED_CLOSER_FLOOR_BASE "dat/vdungeon/floor panels/closer base unpressed.png"
#define VDUNGEON_PRESSED_CLOSER_FLOOR_BASE "dat/vdungeon/floor panels/closer base pressed.png"
#define VDUNGEON_PRESSED_CLOSER_FLOOR_RIGHT "dat/vdungeon/floor panels/closer right pressed.png"

/* sounds */
#define CLOSER_FLOOR_SOUND "dat/digisnd1/door tile pressed 1.ogg"

/* types */
struct closer_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  bool broken;
};

void load_vdungeon_closer_floor (void);
void unload_closer_floor (void);
void load_closer_floor_sounds (void);
void unload_closer_floor_sounds (void);
void register_closer_floor (struct pos p);
struct closer_floor * closer_floor_at_pos (struct pos p);
void press_closer_floor (struct pos p);
void break_closer_floor (struct pos p);
void unpress_closer_floors (void);
void compute_closer_floors (void);
void draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_pressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p);
void draw_unpressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p);
struct coord pressed_closer_floor_left_coord (struct pos p);
struct coord pressed_closer_floor_right_coord (struct pos p);

#endif	/* FREEPOP_CLOSER_FLOOR_H */
