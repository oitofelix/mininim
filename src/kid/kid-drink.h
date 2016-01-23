/*
  kid-drink.h -- kid drink module;

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

#ifndef MININIM_KID_DRINK_H
#define MININIM_KID_DRINK_H

/* bitmaps */
#define KID_DRINK_00 "data/kid/drink/00.png"
#define KID_DRINK_01 "data/kid/drink/01.png"
#define KID_DRINK_02 "data/kid/drink/02.png"
#define KID_DRINK_03 "data/kid/drink/03.png"
#define KID_DRINK_04 "data/kid/drink/04.png"
#define KID_DRINK_05 "data/kid/drink/05.png"
#define KID_DRINK_06 "data/kid/drink/06.png"
#define KID_DRINK_07 "data/kid/drink/07.png"
#define KID_DRINK_08 "data/kid/drink/08.png"
#define KID_DRINK_09 "data/kid/drink/09.png"
#define KID_DRINK_10 "data/kid/drink/10.png"
#define KID_DRINK_11 "data/kid/drink/11.png"
#define KID_DRINK_12 "data/kid/drink/12.png"
#define KID_DRINK_13 "data/kid/drink/13.png"
#define KID_DRINK_14 "data/kid/drink/14.png"

#define KID_DRINK_FRAMESET_NMEMB 15
extern struct frameset kid_drink_frameset[KID_DRINK_FRAMESET_NMEMB];

void load_kid_drink (void);
void unload_kid_drink (void);
void kid_drink (struct anim *k);

extern ALLEGRO_BITMAP *kid_drink_00, *kid_drink_01, *kid_drink_02,
  *kid_drink_03, *kid_drink_04, *kid_drink_05, *kid_drink_06,
  *kid_drink_07, *kid_drink_08, *kid_drink_09, *kid_drink_10,
  *kid_drink_11, *kid_drink_12, *kid_drink_13, *kid_drink_14;

#endif	/* MININIM_KID_DRINK_H */
