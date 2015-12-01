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

#ifndef FREEPOP_SWORD_H
#define FREEPOP_SWORD_H

#define SWORD_FRAMESET_NMEMB 34
extern struct frameset sword_frameset[SWORD_FRAMESET_NMEMB];

/* bitmap resources */
#define NORMAL_SWORD "dat/prince/sword/in the floor/normal.png"
#define SHINY_SWORD "dat/prince/sword/in the floor/bright.png"
#define SWORD_01 "dat/prince/sword/fighting/sword01.png"
#define SWORD_02 "dat/prince/sword/fighting/sword02.png"
#define SWORD_03 "dat/prince/sword/fighting/sword03.png"
#define SWORD_04 "dat/prince/sword/fighting/sword04.png"
#define SWORD_05 "dat/prince/sword/fighting/sword05.png"
#define SWORD_06 "dat/prince/sword/fighting/sword06.png"
#define SWORD_07 "dat/prince/sword/fighting/sword07.png"
#define SWORD_08 "dat/prince/sword/fighting/sword08.png"
#define SWORD_09 "dat/prince/sword/fighting/sword09.png"
#define SWORD_10 "dat/prince/sword/fighting/sword10.png"
#define SWORD_11 "dat/prince/sword/fighting/sword11.png"
#define SWORD_12 "dat/prince/sword/fighting/sword12.png"
#define SWORD_13 "dat/prince/sword/fighting/sword13.png"
#define SWORD_14 "dat/prince/sword/fighting/sword14.png"
#define SWORD_15 "dat/prince/sword/fighting/sword15.png"
#define SWORD_16 "dat/prince/sword/fighting/sword16.png"
#define SWORD_17 "dat/prince/sword/fighting/sword17.png"
#define SWORD_18 "dat/prince/sword/fighting/sword18.png"
#define SWORD_19 "dat/prince/sword/fighting/sword19.png"
#define SWORD_20 "dat/prince/sword/fighting/sword20.png"
#define SWORD_21 "dat/prince/sword/fighting/sword21.png"
#define SWORD_22 "dat/prince/sword/fighting/sword22.png"
#define SWORD_23 "dat/prince/sword/fighting/sword23.png"
#define SWORD_24 "dat/prince/sword/fighting/sword24.png"
#define SWORD_25 "dat/prince/sword/fighting/sword25.png"
#define SWORD_26 "dat/prince/sword/fighting/sword26.png"
#define SWORD_27 "dat/prince/sword/fighting/sword27.png"
#define SWORD_28 "dat/prince/sword/fighting/sword28.png"
#define SWORD_29 "dat/prince/sword/fighting/sword29.png"
#define SWORD_30 "dat/prince/sword/fighting/sword30.png"
#define SWORD_31 "dat/prince/sword/fighting/sword31.png"
#define SWORD_32 "dat/prince/sword/fighting/sword32.png"
#define SWORD_33 "dat/prince/sword/fighting/sword33.png"
#define SWORD_34 "dat/prince/sword/fighting/sword34.png"

void load_sword (void);
void unload_sword (void);
void draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p, int i);
bool is_sword (struct pos *p);

#endif	/* FREEPOP_SWORD_H */
