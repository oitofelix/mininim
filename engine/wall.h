/*
  wall.h -- wall module;

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

#ifndef FREEPOP_WALL_H
#define FREEPOP_WALL_H

#include "physics.h"

/* dungeon vga */
#define DV_WALL_LEFT "data/wall/dv-left.png"
#define DV_WALL_CENTER "data/wall/dv-center.png"
#define DV_WALL_RIGHT "data/wall/dv-right.png"
#define DV_WALL_SINGLE "data/wall/dv-single.png"
#define DV_WALL_FACE "data/wall/dv-face.png"
#define DV_WALL_FACE_TOP "data/wall/dv-face-top.png"
#define DV_WALL_LEFT_BASE "data/wall/dv-left-base.png"
#define DV_WALL_CENTER_BASE "data/wall/dv-center-base.png"
#define DV_WALL_RIGHT_BASE "data/wall/dv-right-base.png"
#define DV_WALL_SINGLE_BASE "data/wall/dv-single-base.png"
#define DV_WALL_RANDOM_BLOCK "data/wall/dv-random-block.png"
#define DV_WALL_WIDE_DIVIDER "data/wall/dv-wide-divider.png"
#define DV_WALL_NARROW_DIVIDER "data/wall/dv-narrow-divider.png"
#define DV_WALL_MARK_TOP_LEFT "data/wall/dv-mark-top-left.png"
#define DV_WALL_MARK_TOP_RIGHT "data/wall/dv-mark-top-right.png"
#define DV_WALL_MARK_BOTTOM_LEFT "data/wall/dv-mark-bottom-left.png"
#define DV_WALL_MARK_BOTTOM_RIGHT "data/wall/dv-mark-bottom-right.png"

/* palace vga */
#define PV_WALL_FACE "data/wall/pv-face.png"
#define PV_WALL_FACE_TOP "data/wall/pv-face-top.png"
#define PV_WALL_MARK_00 "data/wall/pv-mark-00.png"
#define PV_WALL_MARK_01 "data/wall/pv-mark-01.png"
#define PV_WALL_MARK_02 "data/wall/pv-mark-02.png"
#define PV_WALL_MARK_03 "data/wall/pv-mark-03.png"
#define PV_WALL_MARK_04 "data/wall/pv-mark-04.png"
#define PV_WALL_MARK_05 "data/wall/pv-mark-05.png"
#define PV_WALL_MARK_06 "data/wall/pv-mark-06.png"
#define PV_WALL_MARK_07 "data/wall/pv-mark-07.png"
#define PV_WALL_MARK_08 "data/wall/pv-mark-08.png"
#define PV_WALL_MARK_09 "data/wall/pv-mark-09.png"
#define PV_WALL_MARK_10 "data/wall/pv-mark-10.png"
#define PV_WALL_MARK_11 "data/wall/pv-mark-11.png"
#define PV_WALL_MARK_12 "data/wall/pv-mark-12.png"
#define PV_WALL_MARK_13 "data/wall/pv-mark-13.png"
#define PV_WALL_MARK_14 "data/wall/pv-mark-14.png"

void load_wall (void);
void unload_wall (void);
void draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em, enum vm vm);
void draw_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_wall_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_wall_face (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em, enum vm vm);

ALLEGRO_COLOR palace_wall_color (int i);
void compute_palace_wall_color_array (int last_room, int room);
ALLEGRO_COLOR compute_palace_wall_color (struct pos *p, int row, int col);
void draw_pv_wall_brick (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    int row, int col);
struct rect *palace_wall_brick_rect (struct pos *p, int row, int col,
                                     struct rect *r);
ALLEGRO_BITMAP *palace_wall_mark (int i);
struct frame *palace_wall_mark_frame (struct pos *p, int i,
                                      struct frame *f);

void draw_pv_wall (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_pv_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_pv_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p);

void draw_dv_wall (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_sws_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_sws_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_sww_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_sww_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_wws_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_wws_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_www_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_www_base (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r);
void draw_dv_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r);
void draw_dv_wall_random_block (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_divider_00 (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_dv_wall_divider_01 (ALLEGRO_BITMAP *bitmap, struct pos *p);

enum wall_correlation wall_correlation (struct pos *p);
struct coord *wall_base_coord (struct pos *p, struct coord *c);
struct coord *wall_coord (struct pos *p, struct coord *c);
struct coord *wall_face_coord (struct pos *p, struct coord *c);
struct coord *d_wall_face_top_coord (struct pos *p, struct coord *c);
struct coord *p_wall_face_top_coord (struct pos *p, struct coord *c);
struct coord *wall_random_block_coord (struct pos *p, struct coord *c);
struct coord *wall_divider_00_coord (struct pos *p, struct coord *c);
struct coord *wall_divider_01_coord (struct pos *p, struct coord *c);

enum wall_correlation {
  SWS, SWW, WWS, WWW
};

#endif	/* FREEPOP_WALL_H */
