/*
  wall-depedv.h -- wall dungeon ega, palace ega, dungeon vga module;

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

#ifndef MININIM_WALL_DEPEDV_H
#define MININIM_WALL_DEPEDV_H

/* dungeon ega */
#define DE_WALL_LEFT "data/wall/de-left.png"
#define DE_WALL_CENTER "data/wall/de-center.png"
#define DE_WALL_RIGHT "data/wall/de-right.png"
#define DE_WALL_SINGLE "data/wall/de-single.png"
#define DE_WALL_LEFT_BASE "data/wall/de-left-base.png"
#define DE_WALL_CENTER_BASE "data/wall/de-center-base.png"
#define DE_WALL_RIGHT_BASE "data/wall/de-right-base.png"
#define DE_WALL_SINGLE_BASE "data/wall/de-single-base.png"
#define DE_WALL_RANDOM_BLOCK "data/wall/de-random-block.png"
#define DE_WALL_WIDE_DIVIDER "data/wall/de-wide-divider.png"
#define DE_WALL_NARROW_DIVIDER "data/wall/de-narrow-divider.png"
#define DE_WALL_MARK_TOP_LEFT "data/wall/de-mark-top-left.png"
#define DE_WALL_MARK_TOP_RIGHT "data/wall/de-mark-top-right.png"
#define DE_WALL_MARK_BOTTOM_LEFT "data/wall/de-mark-bottom-left.png"
#define DE_WALL_MARK_BOTTOM_RIGHT "data/wall/de-mark-bottom-right.png"

/* palace ega */
#define PE_WALL_LEFT "data/wall/pe-left.png"
#define PE_WALL_CENTER "data/wall/pe-center.png"
#define PE_WALL_RIGHT "data/wall/pe-right.png"
#define PE_WALL_SINGLE "data/wall/pe-single.png"
#define PE_WALL_LEFT_BASE "data/wall/pe-left-base.png"
#define PE_WALL_CENTER_BASE "data/wall/pe-center-base.png"
#define PE_WALL_RIGHT_BASE "data/wall/pe-right-base.png"
#define PE_WALL_SINGLE_BASE "data/wall/pe-single-base.png"
#define PE_WALL_RANDOM_BLOCK "data/wall/pe-random-block.png"
#define PE_WALL_WIDE_DIVIDER "data/wall/pe-wide-divider.png"
#define PE_WALL_NARROW_DIVIDER "data/wall/pe-narrow-divider.png"

/* dungeon vga */
#define DV_WALL_LEFT "data/wall/dv-left.png"
#define DV_WALL_CENTER "data/wall/dv-center.png"
#define DV_WALL_RIGHT "data/wall/dv-right.png"
#define DV_WALL_SINGLE "data/wall/dv-single.png"
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

void load_wall_depedv (void);
void unload_wall_depedv (void);
void draw_wall_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm);
void draw_wall_base_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);
void draw_wall_left_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                              enum em em, enum vm vm);

struct coord *wall_random_block_coord (struct pos *p, struct coord *c);
struct coord *wall_divider_00_coord (struct pos *p, struct coord *c);
struct coord *wall_divider_01_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_WALL_DEPEDV_H */
