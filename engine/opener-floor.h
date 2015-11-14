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

/* constants */
#define OPENER_FLOORS 30

/* sounds */
#define OPENER_FLOOR_SOUND "dat/digisnd1/door tile pressed 1.ogg"

/* types */
struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  int resist_on;
  int resist_off;
};

void load_opener_floor_sounds (void);
void unload_opener_floor_sounds (void);
void register_opener_floor (struct pos p);
struct opener_floor *opener_floor_at_pos (struct pos p);
void draw_opener_floors (void);
bool is_opener_floor_pressed (struct pos p);
void draw_opener_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos p);

#endif	/* FREEPOP_OPENER_FLOOR_H */
