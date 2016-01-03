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

#ifndef MININIM_OPENER_FLOOR_H
#define MININIM_OPENER_FLOOR_H

#include "pos.h"

/* dungeon cga */
#define DC_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/dc-unpressed-base.png"
#define DC_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/dc-unpressed-left.png"
#define DC_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/dc-unpressed-right.png"

/* palace cga */
#define PC_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/pc-unpressed-base.png"
#define PC_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/pc-unpressed-left.png"
#define PC_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/pc-unpressed-right.png"

/* dungeon ega */
#define DE_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/de-unpressed-base.png"
#define DE_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/de-unpressed-left.png"
#define DE_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/de-unpressed-right.png"

/* palace ega */
#define PE_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/pe-unpressed-base.png"
#define PE_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/pe-unpressed-left.png"
#define PE_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/pe-unpressed-right.png"

/* dungeon vga */
#define DV_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/dv-unpressed-base.png"
#define DV_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/dv-unpressed-left.png"
#define DV_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/dv-unpressed-right.png"

/* palace vga */
#define PV_UNPRESSED_OPENER_FLOOR_BASE "data/opener-floor/pv-unpressed-base.png"
#define PV_UNPRESSED_OPENER_FLOOR_LEFT "data/opener-floor/pv-unpressed-left.png"
#define PV_UNPRESSED_OPENER_FLOOR_RIGHT "data/opener-floor/pv-unpressed-right.png"

/* sounds */
#define OPENER_FLOOR_SAMPLE "dat/digisnd1/door tile pressed 1.ogg"

/* types */
struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  bool broken;
};

/* variables */
extern struct opener_floor *opener_floor;
extern size_t opener_floor_nmemb;

/* functions */
void load_opener_floor (void);
void unload_opener_floor (void);
void load_opener_floor_samples (void);
void unload_opener_floor_samples (void);
void register_opener_floor (struct pos *p);
int compare_opener_floors (const void *o0, const void *o1);
struct opener_floor *opener_floor_at_pos (struct pos *p);
void press_opener_floor (struct pos *p);
void break_opener_floor (struct pos *p);
void unpress_opener_floors (void);
void compute_opener_floors (void);
void sample_opener_floors (void);
void draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
void draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                       enum em em, enum vm vm);
void draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm);
void draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                       enum em em, enum vm vm);
void draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                        enum em em, enum vm vm);
struct coord *unpressed_opener_floor_left_coord (struct pos *p, struct coord *c);
struct coord *unpressed_opener_floor_right_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_OPENER_FLOOR_H */
