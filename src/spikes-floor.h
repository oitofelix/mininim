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

#ifndef MININIM_SPIKES_H
#define MININIM_SPIKES_H

/* dungeon cga */
#define DC_SPIKES_FLOOR_LEFT "data/spikes-floor/dc-left.png"
#define DC_SPIKES_FLOOR_RIGHT "data/spikes-floor/dc-right.png"
#define DC_SPIKES_LEFT_00 "data/spikes-floor/dc-spikes-left-00.png"
#define DC_SPIKES_RIGHT_00 "data/spikes-floor/dc-spikes-right-00.png"
#define DC_SPIKES_FG_00 "data/spikes-floor/dc-spikes-fg-00.png"
#define DC_SPIKES_LEFT_01 "data/spikes-floor/dc-spikes-left-01.png"
#define DC_SPIKES_RIGHT_01 "data/spikes-floor/dc-spikes-right-01.png"
#define DC_SPIKES_FG_01 "data/spikes-floor/dc-spikes-fg-01.png"
#define DC_SPIKES_LEFT_02 "data/spikes-floor/dc-spikes-left-02.png"
#define DC_SPIKES_RIGHT_02 "data/spikes-floor/dc-spikes-right-02.png"
#define DC_SPIKES_FG_02 "data/spikes-floor/dc-spikes-fg-02.png"
#define DC_SPIKES_LEFT_03 "data/spikes-floor/dc-spikes-left-03.png"
#define DC_SPIKES_RIGHT_03 "data/spikes-floor/dc-spikes-right-03.png"
#define DC_SPIKES_FG_03 "data/spikes-floor/dc-spikes-fg-03.png"
#define DC_SPIKES_LEFT_04 "data/spikes-floor/dc-spikes-left-04.png"
#define DC_SPIKES_RIGHT_04 "data/spikes-floor/dc-spikes-right-04.png"
#define DC_SPIKES_FG_04 "data/spikes-floor/dc-spikes-fg-04.png"

/* palace cga */
#define PC_SPIKES_FLOOR_LEFT "data/spikes-floor/pc-left.png"
#define PC_SPIKES_FLOOR_RIGHT "data/spikes-floor/pc-right.png"
#define PC_SPIKES_LEFT_00 "data/spikes-floor/pc-spikes-left-00.png"
#define PC_SPIKES_RIGHT_00 "data/spikes-floor/pc-spikes-right-00.png"
#define PC_SPIKES_FG_00 "data/spikes-floor/pc-spikes-fg-00.png"
#define PC_SPIKES_LEFT_01 "data/spikes-floor/pc-spikes-left-01.png"
#define PC_SPIKES_RIGHT_01 "data/spikes-floor/pc-spikes-right-01.png"
#define PC_SPIKES_FG_01 "data/spikes-floor/pc-spikes-fg-01.png"
#define PC_SPIKES_LEFT_02 "data/spikes-floor/pc-spikes-left-02.png"
#define PC_SPIKES_RIGHT_02 "data/spikes-floor/pc-spikes-right-02.png"
#define PC_SPIKES_FG_02 "data/spikes-floor/pc-spikes-fg-02.png"
#define PC_SPIKES_LEFT_03 "data/spikes-floor/pc-spikes-left-03.png"
#define PC_SPIKES_RIGHT_03 "data/spikes-floor/pc-spikes-right-03.png"
#define PC_SPIKES_FG_03 "data/spikes-floor/pc-spikes-fg-03.png"
#define PC_SPIKES_LEFT_04 "data/spikes-floor/pc-spikes-left-04.png"
#define PC_SPIKES_RIGHT_04 "data/spikes-floor/pc-spikes-right-04.png"
#define PC_SPIKES_FG_04 "data/spikes-floor/pc-spikes-fg-04.png"

/* dungeon ega */
#define DE_SPIKES_FLOOR_LEFT "data/spikes-floor/de-left.png"
#define DE_SPIKES_FLOOR_RIGHT "data/spikes-floor/de-right.png"
#define DE_SPIKES_LEFT_00 "data/spikes-floor/de-spikes-left-00.png"
#define DE_SPIKES_RIGHT_00 "data/spikes-floor/de-spikes-right-00.png"
#define DE_SPIKES_FG_00 "data/spikes-floor/de-spikes-fg-00.png"
#define DE_SPIKES_LEFT_01 "data/spikes-floor/de-spikes-left-01.png"
#define DE_SPIKES_RIGHT_01 "data/spikes-floor/de-spikes-right-01.png"
#define DE_SPIKES_FG_01 "data/spikes-floor/de-spikes-fg-01.png"
#define DE_SPIKES_LEFT_02 "data/spikes-floor/de-spikes-left-02.png"
#define DE_SPIKES_RIGHT_02 "data/spikes-floor/de-spikes-right-02.png"
#define DE_SPIKES_FG_02 "data/spikes-floor/de-spikes-fg-02.png"
#define DE_SPIKES_LEFT_03 "data/spikes-floor/de-spikes-left-03.png"
#define DE_SPIKES_RIGHT_03 "data/spikes-floor/de-spikes-right-03.png"
#define DE_SPIKES_FG_03 "data/spikes-floor/de-spikes-fg-03.png"
#define DE_SPIKES_LEFT_04 "data/spikes-floor/de-spikes-left-04.png"
#define DE_SPIKES_RIGHT_04 "data/spikes-floor/de-spikes-right-04.png"
#define DE_SPIKES_FG_04 "data/spikes-floor/de-spikes-fg-04.png"

/* palace ega */
#define PE_SPIKES_FLOOR_LEFT "data/spikes-floor/pe-left.png"
#define PE_SPIKES_FLOOR_RIGHT "data/spikes-floor/pe-right.png"
#define PE_SPIKES_LEFT_00 "data/spikes-floor/pe-spikes-left-00.png"
#define PE_SPIKES_RIGHT_00 "data/spikes-floor/pe-spikes-right-00.png"
#define PE_SPIKES_FG_00 "data/spikes-floor/pe-spikes-fg-00.png"
#define PE_SPIKES_LEFT_01 "data/spikes-floor/pe-spikes-left-01.png"
#define PE_SPIKES_RIGHT_01 "data/spikes-floor/pe-spikes-right-01.png"
#define PE_SPIKES_FG_01 "data/spikes-floor/pe-spikes-fg-01.png"
#define PE_SPIKES_LEFT_02 "data/spikes-floor/pe-spikes-left-02.png"
#define PE_SPIKES_RIGHT_02 "data/spikes-floor/pe-spikes-right-02.png"
#define PE_SPIKES_FG_02 "data/spikes-floor/pe-spikes-fg-02.png"
#define PE_SPIKES_LEFT_03 "data/spikes-floor/pe-spikes-left-03.png"
#define PE_SPIKES_RIGHT_03 "data/spikes-floor/pe-spikes-right-03.png"
#define PE_SPIKES_FG_03 "data/spikes-floor/pe-spikes-fg-03.png"
#define PE_SPIKES_LEFT_04 "data/spikes-floor/pe-spikes-left-04.png"
#define PE_SPIKES_RIGHT_04 "data/spikes-floor/pe-spikes-right-04.png"
#define PE_SPIKES_FG_04 "data/spikes-floor/pe-spikes-fg-04.png"

/* dungeon vga */
#define DV_SPIKES_FLOOR_LEFT "data/spikes-floor/dv-left.png"
#define DV_SPIKES_FLOOR_RIGHT "data/spikes-floor/dv-right.png"
#define DV_SPIKES_LEFT_00 "data/spikes-floor/dv-spikes-left-00.png"
#define DV_SPIKES_RIGHT_00 "data/spikes-floor/dv-spikes-right-00.png"
#define DV_SPIKES_FG_00 "data/spikes-floor/dv-spikes-fg-00.png"
#define DV_SPIKES_LEFT_01 "data/spikes-floor/dv-spikes-left-01.png"
#define DV_SPIKES_RIGHT_01 "data/spikes-floor/dv-spikes-right-01.png"
#define DV_SPIKES_FG_01 "data/spikes-floor/dv-spikes-fg-01.png"
#define DV_SPIKES_LEFT_02 "data/spikes-floor/dv-spikes-left-02.png"
#define DV_SPIKES_RIGHT_02 "data/spikes-floor/dv-spikes-right-02.png"
#define DV_SPIKES_FG_02 "data/spikes-floor/dv-spikes-fg-02.png"
#define DV_SPIKES_LEFT_03 "data/spikes-floor/dv-spikes-left-03.png"
#define DV_SPIKES_RIGHT_03 "data/spikes-floor/dv-spikes-right-03.png"
#define DV_SPIKES_FG_03 "data/spikes-floor/dv-spikes-fg-03.png"
#define DV_SPIKES_LEFT_04 "data/spikes-floor/dv-spikes-left-04.png"
#define DV_SPIKES_RIGHT_04 "data/spikes-floor/dv-spikes-right-04.png"
#define DV_SPIKES_FG_04 "data/spikes-floor/dv-spikes-fg-04.png"

/* palace vga */
#define PV_SPIKES_FLOOR_LEFT "data/spikes-floor/pv-left.png"
#define PV_SPIKES_FLOOR_RIGHT "data/spikes-floor/pv-right.png"
#define PV_SPIKES_LEFT_00 "data/spikes-floor/pv-spikes-left-00.png"
#define PV_SPIKES_RIGHT_00 "data/spikes-floor/pv-spikes-right-00.png"
#define PV_SPIKES_FG_00 "data/spikes-floor/pv-spikes-fg-00.png"
#define PV_SPIKES_LEFT_01 "data/spikes-floor/pv-spikes-left-01.png"
#define PV_SPIKES_RIGHT_01 "data/spikes-floor/pv-spikes-right-01.png"
#define PV_SPIKES_FG_01 "data/spikes-floor/pv-spikes-fg-01.png"
#define PV_SPIKES_LEFT_02 "data/spikes-floor/pv-spikes-left-02.png"
#define PV_SPIKES_RIGHT_02 "data/spikes-floor/pv-spikes-right-02.png"
#define PV_SPIKES_FG_02 "data/spikes-floor/pv-spikes-fg-02.png"
#define PV_SPIKES_LEFT_03 "data/spikes-floor/pv-spikes-left-03.png"
#define PV_SPIKES_RIGHT_03 "data/spikes-floor/pv-spikes-right-03.png"
#define PV_SPIKES_FG_03 "data/spikes-floor/pv-spikes-fg-03.png"
#define PV_SPIKES_LEFT_04 "data/spikes-floor/pv-spikes-left-04.png"
#define PV_SPIKES_RIGHT_04 "data/spikes-floor/pv-spikes-right-04.png"
#define PV_SPIKES_FG_04 "data/spikes-floor/pv-spikes-fg-04.png"

/* variables */
extern struct spikes_floor *spikes_floor;
extern size_t spikes_floor_nmemb;

/* functions */
void load_spikes_floor (void);
void unload_spikes_floor (void);
void register_spikes_floor (struct pos *p);
void sort_spikes_floors (void);
int compare_spikes_floors (const void *s0, const void *s1);
struct spikes_floor * spikes_floor_at_pos (struct pos *p);
void remove_spikes_floor (struct spikes_floor *s);
void break_spikes_floor (struct pos *p);
void reset_murder_spikes_floor (int id);
void compute_spikes_floors (void);
bool should_spikes_raise_for_pos (struct pos *p, struct pos *pk);
bool should_spikes_raise (struct pos *p);
void draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
void draw_spikes_floor_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
void draw_spikes_floor_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                   enum em em, enum vm vm);
void draw_spikes_floor_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                    enum em em, enum vm vm);
void draw_spikes (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  struct spikes_floor *s, enum em em, enum vm vm);
void draw_spikes_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       struct spikes_floor *s, enum em em, enum vm vm);
void draw_spikes_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        struct spikes_floor *s, enum em em, enum vm vm);
void draw_spikes_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, enum em em, enum vm vm);
void draw_spikes_00 (ALLEGRO_BITMAP *bitmap, struct pos *p, enum em em, enum vm vm);
void draw_spikes_left_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_spikes_right_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_spikes_fg_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *d_spikes_left_00_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_left_00_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_right_00_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_right_00_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_fg_00_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_fg_00_coord (struct pos *p, struct coord *c);
void draw_spikes_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_spikes_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_spikes_right_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_spikes_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *d_spikes_left_01_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_left_01_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_right_01_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_right_01_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_fg_01_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_fg_01_coord (struct pos *p, struct coord *c);
void draw_spikes_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_spikes_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_spikes_right_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_spikes_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *d_spikes_left_02_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_left_02_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_right_02_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_right_02_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_fg_02_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_fg_02_coord (struct pos *p, struct coord *c);
void draw_spikes_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_spikes_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_spikes_right_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_spikes_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *d_spikes_left_03_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_left_03_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_right_03_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_right_03_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_fg_03_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_fg_03_coord (struct pos *p, struct coord *c);
void draw_spikes_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_spikes_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_spikes_right_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_spikes_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
struct coord *d_spikes_left_04_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_left_04_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_right_04_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_right_04_coord (struct pos *p, struct coord *c);
struct coord *d_spikes_fg_04_coord (struct pos *p, struct coord *c);
struct coord *p_spikes_fg_04_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_SPIKES_H */
