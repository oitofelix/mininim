/*
  balcony.h -- balcony module;

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

#ifndef MININIM_BALCONY_H
#define MININIM_BALCONY_H

/* dungeon cga */
#define DC_BALCONY_TOP_LEFT "data/balcony/dc-top-left.png"
#define DC_BALCONY_TOP_RIGHT "data/balcony/dc-top-right.png"
#define DC_BALCONY_BOTTOM_LEFT "data/balcony/dc-bottom-left.png"
#define DC_BALCONY_BOTTOM_RIGHT "data/balcony/dc-bottom-right.png"

/* palace cga */
#define PC_BALCONY_TOP_LEFT "data/balcony/pc-top-left.png"
#define PC_BALCONY_TOP_RIGHT "data/balcony/pc-top-right.png"
#define PC_BALCONY_BOTTOM_LEFT "data/balcony/pc-bottom-left.png"
#define PC_BALCONY_BOTTOM_RIGHT "data/balcony/pc-bottom-right.png"

/* dungeon ega */
#define DE_BALCONY_TOP_LEFT "data/balcony/de-top-left.png"
#define DE_BALCONY_TOP_RIGHT "data/balcony/de-top-right.png"
#define DE_BALCONY_BOTTOM_LEFT "data/balcony/de-bottom-left.png"
#define DE_BALCONY_BOTTOM_RIGHT "data/balcony/de-bottom-right.png"

/* palace ega */
#define PE_BALCONY_TOP_LEFT "data/balcony/pe-top-left.png"
#define PE_BALCONY_TOP_RIGHT "data/balcony/pe-top-right.png"
#define PE_BALCONY_BOTTOM_LEFT "data/balcony/pe-bottom-left.png"
#define PE_BALCONY_BOTTOM_RIGHT "data/balcony/pe-bottom-right.png"

/* dungeon vga */
#define DV_BALCONY_TOP_LEFT "data/balcony/dv-top-left.png"
#define DV_BALCONY_TOP_RIGHT "data/balcony/dv-top-right.png"
#define DV_BALCONY_BOTTOM_LEFT "data/balcony/dv-bottom-left.png"
#define DV_BALCONY_BOTTOM_RIGHT "data/balcony/dv-bottom-right.png"

/* palace vga */
#define PV_BALCONY_TOP_LEFT "data/balcony/pv-top-left.png"
#define PV_BALCONY_TOP_RIGHT "data/balcony/pv-top-right.png"
#define PV_BALCONY_BOTTOM_LEFT "data/balcony/pv-bottom-left.png"
#define PV_BALCONY_BOTTOM_RIGHT "data/balcony/pv-bottom-right.png"

void load_balcony (void);
void unload_balcony (void);
void draw_balcony (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm);
struct coord *balcony_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *balcony_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *balcony_top_left_coord (struct pos *p, struct coord *c);
struct coord *balcony_top_right_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_BALCONY_H */
