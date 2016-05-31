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

#ifndef MININIM_CLOSER_FLOOR_H
#define MININIM_CLOSER_FLOOR_H

/* dungeon cga */
#define DC_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/dc-unpressed-base.png"
#define DC_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/dc-pressed-base.png"
#define DC_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/dc-pressed-right.png"

/* palace cga */
#define PC_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pc-unpressed-base.png"
#define PC_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pc-pressed-base.png"
#define PC_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/pc-pressed-right.png"

/* dungeon ega */
#define DE_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/de-unpressed-base.png"
#define DE_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/de-pressed-base.png"
#define DE_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/de-pressed-right.png"

/* palace ega */
#define PE_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pe-unpressed-base.png"
#define PE_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pe-pressed-base.png"
#define PE_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/pe-pressed-right.png"

/* dungeon vga */
#define DV_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/dv-unpressed-base.png"
#define DV_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/dv-pressed-base.png"
#define DV_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/dv-pressed-right.png"

/* palace vga */
#define PV_UNPRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pv-unpressed-base.png"
#define PV_PRESSED_CLOSER_FLOOR_BASE "data/closer-floor/pv-pressed-base.png"
#define PV_PRESSED_CLOSER_FLOOR_RIGHT "data/closer-floor/pv-pressed-right.png"

/* variables */
extern struct closer_floor *closer_floor;
extern size_t closer_floor_nmemb;

/* functions */
void load_closer_floor (void);
void unload_closer_floor (void);
void register_closer_floor (struct pos *p);
int compare_closer_floors (const void *c0, const void *c1);
struct closer_floor * closer_floor_at_pos (struct pos *p);
struct closer_floor *closer_floor_by_event (struct pos *p, int event, int dir);
void remove_closer_floor (struct closer_floor *c);
void press_closer_floor (struct pos *p);
void break_closer_floor (struct pos *p);
void unpress_closer_floors (void);
void register_changed_closer_floors (void);
void compute_closer_floors (void);
void draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
void draw_pressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
void draw_pressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                     enum em em, enum vm vm);
void draw_pressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                     enum em em, enum vm vm);
void draw_pressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                      enum em em, enum vm vm);
void draw_unpressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm);
void draw_unpressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                       enum em em, enum vm vm);
struct coord *pressed_closer_floor_left_coord (struct pos *p, struct coord *c);
struct coord *pressed_closer_floor_right_coord (struct pos *p, struct coord *c);
void draw_closer_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);

#endif	/* MININIM_CLOSER_FLOOR_H */
