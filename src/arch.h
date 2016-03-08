/*
  arch.h -- arch module;

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

#ifndef MININIM_ARCH_H
#define MININIM_ARCH_H

/* dungeon cga */
#define DC_ARCH_BOTTOM "data/arch/dc-bottom.png"
#define DC_ARCH_TOP_LEFT "data/arch/dc-top-left.png"
#define DC_ARCH_TOP_LEFT_END "data/arch/dc-top-left-end.png"
#define DC_ARCH_TOP_RIGHT "data/arch/dc-top-right.png"
#define DC_ARCH_TOP_RIGHT_END "data/arch/dc-top-right-end.png"
#define DC_ARCH_TOP_MID "data/arch/dc-top-mid.png"
#define DC_ARCH_TOP_SMALL "data/arch/dc-top-small.png"
#define DC_ARCH_TOP_TOP "data/arch/dc-top-top.png"

/* palace cga */
#define PC_ARCH_BOTTOM "data/arch/pc-bottom.png"
#define PC_ARCH_TOP_LEFT "data/arch/pc-top-left.png"
#define PC_ARCH_TOP_LEFT_END "data/arch/pc-top-left-end.png"
#define PC_ARCH_TOP_RIGHT "data/arch/pc-top-right.png"
#define PC_ARCH_TOP_RIGHT_END "data/arch/pc-top-right-end.png"
#define PC_ARCH_TOP_MID "data/arch/pc-top-mid.png"
#define PC_ARCH_TOP_SMALL "data/arch/pc-top-small.png"
#define PC_ARCH_TOP_TOP "data/arch/pc-top-top.png"

/* dungeon ega */
#define DE_ARCH_BOTTOM "data/arch/de-bottom.png"
#define DE_ARCH_TOP_LEFT "data/arch/de-top-left.png"
#define DE_ARCH_TOP_LEFT_END "data/arch/de-top-left-end.png"
#define DE_ARCH_TOP_RIGHT "data/arch/de-top-right.png"
#define DE_ARCH_TOP_RIGHT_END "data/arch/de-top-right-end.png"
#define DE_ARCH_TOP_MID "data/arch/de-top-mid.png"
#define DE_ARCH_TOP_SMALL "data/arch/de-top-small.png"
#define DE_ARCH_TOP_TOP "data/arch/de-top-top.png"

/* palace ega */
#define PE_ARCH_BOTTOM "data/arch/pe-bottom.png"
#define PE_ARCH_TOP_LEFT "data/arch/pe-top-left.png"
#define PE_ARCH_TOP_LEFT_END "data/arch/pe-top-left-end.png"
#define PE_ARCH_TOP_RIGHT "data/arch/pe-top-right.png"
#define PE_ARCH_TOP_RIGHT_END "data/arch/pe-top-right-end.png"
#define PE_ARCH_TOP_MID "data/arch/pe-top-mid.png"
#define PE_ARCH_TOP_SMALL "data/arch/pe-top-small.png"
#define PE_ARCH_TOP_TOP "data/arch/pe-top-top.png"

/* dungeon vga */
#define DV_ARCH_BOTTOM "data/arch/dv-bottom.png"
#define DV_ARCH_TOP_LEFT "data/arch/dv-top-left.png"
#define DV_ARCH_TOP_LEFT_END "data/arch/dv-top-left-end.png"
#define DV_ARCH_TOP_RIGHT "data/arch/dv-top-right.png"
#define DV_ARCH_TOP_RIGHT_END "data/arch/dv-top-right-end.png"
#define DV_ARCH_TOP_MID "data/arch/dv-top-mid.png"
#define DV_ARCH_TOP_SMALL "data/arch/dv-top-small.png"
#define DV_ARCH_TOP_TOP "data/arch/dv-top-top.png"

/* palace vga */
#define PV_ARCH_BOTTOM "data/arch/pv-bottom.png"
#define PV_ARCH_TOP_LEFT "data/arch/pv-top-left.png"
#define PV_ARCH_TOP_LEFT_END "data/arch/pv-top-left-end.png"
#define PV_ARCH_TOP_RIGHT "data/arch/pv-top-right.png"
#define PV_ARCH_TOP_RIGHT_END "data/arch/pv-top-right-end.png"
#define PV_ARCH_TOP_MID "data/arch/pv-top-mid.png"
#define PV_ARCH_TOP_SMALL "data/arch/pv-top-small.png"
#define PV_ARCH_TOP_TOP "data/arch/pv-top-top.png"

void load_arch (void);
void unload_arch (void);
void draw_arch_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm);
void draw_arch_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_arch_top_mid (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_arch_top_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_arch_top_small (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_arch_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm);
void draw_arch_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_arch_top_left_end (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
void draw_arch_top_right_end (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);

struct coord *arch_bottom_coord (struct pos *p, struct coord *c);
struct coord *arch_top_coord (struct pos *p, struct coord *c);
struct coord *arch_top_top_coord (struct pos *p, struct coord *c);
struct coord *arch_top_left_end_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_ARCH_H */
