/*
  sword.h -- sword module;

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

#ifndef MININIM_SWORD_H
#define MININIM_SWORD_H

#define SWORD_FRAMESET_NMEMB 34
extern struct frameset sword_frameset[SWORD_FRAMESET_NMEMB];

/* cga */
#define C_NORMAL_SWORD "data/sword/c-normal.png"
#define C_SHINY_SWORD "data/sword/c-shiny.png"

/* ega */
#define E_NORMAL_SWORD "data/sword/e-normal.png"
#define E_SHINY_SWORD "data/sword/e-shiny.png"

/* vga */
#define V_NORMAL_SWORD "data/sword/v-normal.png"
#define V_SHINY_SWORD "data/sword/v-shiny.png"

/* palettable */
#define SWORD_00 "data/sword/00.png"
#define SWORD_01 "data/sword/01.png"
#define SWORD_02 "data/sword/02.png"
#define SWORD_03 "data/sword/03.png"
#define SWORD_04 "data/sword/04.png"
#define SWORD_05 "data/sword/05.png"
#define SWORD_06 "data/sword/06.png"
#define SWORD_07 "data/sword/07.png"
#define SWORD_08 "data/sword/08.png"
#define SWORD_09 "data/sword/09.png"
#define SWORD_10 "data/sword/10.png"
#define SWORD_11 "data/sword/11.png"
#define SWORD_12 "data/sword/12.png"
#define SWORD_13 "data/sword/13.png"
#define SWORD_14 "data/sword/14.png"
#define SWORD_15 "data/sword/15.png"
#define SWORD_16 "data/sword/16.png"
#define SWORD_17 "data/sword/17.png"
#define SWORD_18 "data/sword/18.png"
#define SWORD_19 "data/sword/19.png"
#define SWORD_20 "data/sword/20.png"
#define SWORD_21 "data/sword/21.png"
#define SWORD_22 "data/sword/22.png"
#define SWORD_23 "data/sword/23.png"
#define SWORD_24 "data/sword/24.png"
#define SWORD_25 "data/sword/25.png"
#define SWORD_26 "data/sword/26.png"
#define SWORD_27 "data/sword/27.png"
#define SWORD_28 "data/sword/28.png"
#define SWORD_29 "data/sword/29.png"
#define SWORD_30 "data/sword/30.png"
#define SWORD_31 "data/sword/31.png"
#define SWORD_32 "data/sword/32.png"
#define SWORD_33 "data/sword/33.png"

void load_sword (void);
void unload_sword (void);
void draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p, enum vm vm);
bool is_sword (struct pos *p);

#endif	/* MININIM_SWORD_H */
