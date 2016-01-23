/*
  potion.h -- potion module;

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

#ifndef MININIM_POTION_H
#define MININIM_POTION_H

/* dungeon cga */
#define DC_SMALL_POTION "data/potion/dc-small.png"
#define DC_BIG_POTION "data/potion/dc-big.png"

/* palace cga */
#define PC_SMALL_POTION "data/potion/pc-small.png"
#define PC_BIG_POTION "data/potion/pc-big.png"

/* dungeon ega */
#define DE_SMALL_POTION "data/potion/de-small.png"
#define DE_BIG_POTION "data/potion/de-big.png"

/* palace ega */
#define PE_SMALL_POTION "data/potion/pe-small.png"
#define PE_BIG_POTION "data/potion/pe-big.png"

/* dungeon vga */
#define DV_SMALL_POTION "data/potion/dv-small.png"
#define DV_BIG_POTION "data/potion/dv-big.png"

/* palace vga */
#define PV_SMALL_POTION "data/potion/pv-small.png"
#define PV_BIG_POTION "data/potion/pv-big.png"

/* palettable */
#define BUBBLE_00 "data/potion/bubble-00.png"
#define BUBBLE_01 "data/potion/bubble-01.png"
#define BUBBLE_02 "data/potion/bubble-02.png"
#define BUBBLE_03 "data/potion/bubble-03.png"
#define BUBBLE_04 "data/potion/bubble-04.png"
#define BUBBLE_05 "data/potion/bubble-05.png"
#define BUBBLE_06 "data/potion/bubble-06.png"

void load_potion (void);
void unload_potion (void);
void draw_potion (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm);
bool is_potion (struct pos *p);

#endif	/* MININIM_POTION_H */
