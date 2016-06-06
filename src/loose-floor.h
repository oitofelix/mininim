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

#ifndef MININIM_LOOSE_FLOOR_H
#define MININIM_LOOSE_FLOOR_H

/* dungeon cga */
#define DC_LOOSE_FLOOR_BASE_00 "data/loose-floor/dc-base-00.png"
#define DC_LOOSE_FLOOR_LEFT_00 "data/loose-floor/dc-left-00.png"
#define DC_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/dc-right-00.png"
#define DC_LOOSE_FLOOR_BASE_01 "data/loose-floor/dc-base-01.png"
#define DC_LOOSE_FLOOR_LEFT_01 "data/loose-floor/dc-left-01.png"
#define DC_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/dc-right-01.png"

/* palace cga */
#define PC_LOOSE_FLOOR_BASE_00 "data/loose-floor/pc-base-00.png"
#define PC_LOOSE_FLOOR_LEFT_00 "data/loose-floor/pc-left-00.png"
#define PC_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/pc-right-00.png"
#define PC_LOOSE_FLOOR_BASE_01 "data/loose-floor/pc-base-01.png"
#define PC_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pc-left-01.png"
#define PC_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pc-right-01.png"

/* dungeon ega */
#define DE_LOOSE_FLOOR_BASE_00 "data/loose-floor/de-base-00.png"
#define DE_LOOSE_FLOOR_LEFT_00 "data/loose-floor/de-left-00.png"
#define DE_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/de-right-00.png"
#define DE_LOOSE_FLOOR_BASE_01 "data/loose-floor/de-base-01.png"
#define DE_LOOSE_FLOOR_LEFT_01 "data/loose-floor/de-left-01.png"
#define DE_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/de-right-01.png"

/* palace ega */
#define PE_LOOSE_FLOOR_BASE_00 "data/loose-floor/pe-base-00.png"
#define PE_LOOSE_FLOOR_LEFT_00 "data/loose-floor/pe-left-00.png"
#define PE_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/pe-right-00.png"
#define PE_LOOSE_FLOOR_BASE_01 "data/loose-floor/pe-base-01.png"
#define PE_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pe-left-01.png"
#define PE_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pe-right-01.png"

/* dungeon vga */
#define DV_LOOSE_FLOOR_BASE_00 "data/loose-floor/dv-base-00.png"
#define DV_LOOSE_FLOOR_LEFT_00 "data/loose-floor/dv-left-00.png"
#define DV_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/dv-right-00.png"
#define DV_LOOSE_FLOOR_BASE_01 "data/loose-floor/dv-base-01.png"
#define DV_LOOSE_FLOOR_LEFT_01 "data/loose-floor/dv-left-01.png"
#define DV_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/dv-right-01.png"

/* palace vga */
#define PV_LOOSE_FLOOR_BASE_00 "data/loose-floor/pv-base-00.png"
#define PV_LOOSE_FLOOR_LEFT_00 "data/loose-floor/pv-left-00.png"
#define PV_LOOSE_FLOOR_RIGHT_00 "data/loose-floor/pv-right-00.png"
#define PV_LOOSE_FLOOR_BASE_01 "data/loose-floor/pv-base-01.png"
#define PV_LOOSE_FLOOR_LEFT_01 "data/loose-floor/pv-left-01.png"
#define PV_LOOSE_FLOOR_RIGHT_01 "data/loose-floor/pv-right-01.png"

/* variables */
extern struct loose_floor *loose_floor;
extern size_t loose_floor_nmemb;

/* functions */
void load_loose_floor (void);
void unload_loose_floor (void);
ALLEGRO_BITMAP *create_loose_floor_01_bitmap (enum em em, enum vm vm);
void register_loose_floor (struct pos *p);
void sort_loose_floors (void);
int compare_loose_floors (const void *l0, const void *l1);
struct loose_floor * loose_floor_at_pos (struct pos *p);
void remove_loose_floor (struct loose_floor *l);
void release_loose_floor (struct pos *p);
void compute_loose_floors (void);
void compute_loose_floor_shake (struct loose_floor *l);
void compute_loose_floor_release (struct loose_floor *l);
void compute_loose_floor_fall (struct loose_floor *l);
void shake_loose_floor_row (struct pos *p);
void sample_random_loose_floor (struct pos *p);
ALLEGRO_BITMAP *get_correct_falling_loose_floor_bitmap (ALLEGRO_BITMAP *b);
void draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm);
void draw_loose_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                            enum em em, enum vm vm);
void draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                            enum em em, enum vm vm);
void draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_loose_floor_00_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_00_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_00_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
void draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_loose_floor_01_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
struct coord *loose_floor_left_coord (struct pos *p, struct coord *c);
struct coord *loose_floor_right_coord (struct pos *p, struct coord *c);
void loose_floor_fall_debug (void);

#endif	/* MININIM_LOOSE_FLOOR_H */
