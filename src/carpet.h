/*
  carpet.h -- carpet module;

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

#ifndef MININIM_CARPET_H
#define MININIM_CARPET_H

/* dungeon cga */
#define DC_CARPET_00 "data/carpet/dc-00.png"
#define DC_CARPET_TOP_00 "data/carpet/dc-top-00.png"
#define DC_CARPET_01 "data/carpet/dc-01.png"
#define DC_CARPET_TOP_01 "data/carpet/dc-top-01.png"

/* palace cga */
#define PC_CARPET_00 "data/carpet/pc-00.png"
#define PC_CARPET_TOP_00 "data/carpet/pc-top-00.png"
#define PC_CARPET_01 "data/carpet/pc-01.png"
#define PC_CARPET_TOP_01 "data/carpet/pc-top-01.png"

/* dungeon ega */
#define DE_CARPET_00 "data/carpet/de-00.png"
#define DE_CARPET_TOP_00 "data/carpet/de-top-00.png"
#define DE_CARPET_01 "data/carpet/de-01.png"
#define DE_CARPET_TOP_01 "data/carpet/de-top-01.png"

/* palace ega */
#define PE_CARPET_00 "data/carpet/pe-00.png"
#define PE_CARPET_TOP_00 "data/carpet/pe-top-00.png"
#define PE_CARPET_01 "data/carpet/pe-01.png"
#define PE_CARPET_TOP_01 "data/carpet/pe-top-01.png"

/* dungeon vga */
#define DV_CARPET_00 "data/carpet/dv-00.png"
#define DV_CARPET_TOP_00 "data/carpet/dv-top-00.png"
#define DV_CARPET_01 "data/carpet/dv-01.png"
#define DV_CARPET_TOP_01 "data/carpet/dv-top-01.png"

/* palace vga */
#define PV_CARPET_00 "data/carpet/pv-00.png"
#define PV_CARPET_TOP_00 "data/carpet/pv-top-00.png"
#define PV_CARPET_01 "data/carpet/pv-01.png"
#define PV_CARPET_TOP_01 "data/carpet/pv-top-01.png"

void load_carpet (void);
void unload_carpet (void);
void draw_carpet_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_carpet_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_carpet_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     struct frame *f, enum em em, enum vm vm);
struct coord *carpet_coord (struct pos *p, struct coord *c);
struct coord *carpet_top_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_CARPET_H */
