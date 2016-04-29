/*
  big-pillar.h -- big pillar module;

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

#ifndef MININIM_BIG_PILLAR_H
#define MININIM_BIG_PILLAR_H

/* dungeon cga */
#define DC_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/dc-bottom-left.png"
#define DC_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/dc-bottom-right.png"
#define DC_BIG_PILLAR_TOP_LEFT "data/big-pillar/dc-top-left.png"
#define DC_BIG_PILLAR_TOP_RIGHT "data/big-pillar/dc-top-right.png"
#define DC_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/dc-top-right-top.png"
#define DC_BIG_PILLAR_BOTTOM_FG "data/big-pillar/dc-bottom-fg.png"

/* palace cga */
#define PC_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/pc-bottom-left.png"
#define PC_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/pc-bottom-right.png"
#define PC_BIG_PILLAR_TOP_LEFT "data/big-pillar/pc-top-left.png"
#define PC_BIG_PILLAR_TOP_RIGHT "data/big-pillar/pc-top-right.png"
#define PC_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/pc-top-right-top.png"
#define PC_BIG_PILLAR_BOTTOM_FG "data/big-pillar/pc-bottom-fg.png"

/* dungeon ega */
#define DE_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/de-bottom-left.png"
#define DE_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/de-bottom-right.png"
#define DE_BIG_PILLAR_TOP_LEFT "data/big-pillar/de-top-left.png"
#define DE_BIG_PILLAR_TOP_RIGHT "data/big-pillar/de-top-right.png"
#define DE_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/de-top-right-top.png"
#define DE_BIG_PILLAR_BOTTOM_FG "data/big-pillar/de-bottom-fg.png"

/* palace ega */
#define PE_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/pe-bottom-left.png"
#define PE_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/pe-bottom-right.png"
#define PE_BIG_PILLAR_TOP_LEFT "data/big-pillar/pe-top-left.png"
#define PE_BIG_PILLAR_TOP_RIGHT "data/big-pillar/pe-top-right.png"
#define PE_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/pe-top-right-top.png"
#define PE_BIG_PILLAR_BOTTOM_FG "data/big-pillar/pe-bottom-fg.png"

/* dungeon vga */
#define DV_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/dv-bottom-left.png"
#define DV_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/dv-bottom-right.png"
#define DV_BIG_PILLAR_TOP_LEFT "data/big-pillar/dv-top-left.png"
#define DV_BIG_PILLAR_TOP_RIGHT "data/big-pillar/dv-top-right.png"
#define DV_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/dv-top-right-top.png"
#define DV_BIG_PILLAR_BOTTOM_FG "data/big-pillar/dv-bottom-fg.png"

/* palace vga */
#define PV_BIG_PILLAR_BOTTOM_LEFT "data/big-pillar/pv-bottom-left.png"
#define PV_BIG_PILLAR_BOTTOM_RIGHT "data/big-pillar/pv-bottom-right.png"
#define PV_BIG_PILLAR_TOP_LEFT "data/big-pillar/pv-top-left.png"
#define PV_BIG_PILLAR_TOP_RIGHT "data/big-pillar/pv-top-right.png"
#define PV_BIG_PILLAR_TOP_RIGHT_TOP "data/big-pillar/pv-top-right-top.png"
#define PV_BIG_PILLAR_BOTTOM_FG "data/big-pillar/pv-bottom-fg.png"

void load_big_pillar (void);
void unload_big_pillar (void);
void draw_big_pillar_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_big_pillar_bottom_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm);
void draw_big_pillar_bottom_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                   enum em em, enum vm vm);
void draw_big_pillar_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
void draw_big_pillar_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_big_pillar_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
void draw_big_pillar_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
void draw_big_pillar_top_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
struct coord *big_pillar_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_top_left_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_top_right_coord (struct pos *p, struct coord *c);
struct coord *d_big_pillar_top_right_top_coord (struct pos *p, struct coord *c);
struct coord *p_big_pillar_top_right_top_coord (struct pos *p, struct coord *c);
struct coord *big_pillar_bottom_fg_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_BIG_PILLAR_H */
