/*
  kid-vjump.h -- kid vjump module;

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

#ifndef MININIM_KID_VJUMP_H
#define MININIM_KID_VJUMP_H

/* bitmaps */
#define KID_VJUMP_01 "dat/kid/scaling/frame01.png"
#define KID_VJUMP_02 "dat/kid/scaling/frame02.png"
#define KID_VJUMP_03 "dat/kid/scaling/frame03.png"
#define KID_VJUMP_04 "dat/kid/scaling/frame04.png"
#define KID_VJUMP_05 "dat/kid/scaling/frame05.png"
#define KID_VJUMP_06 "dat/kid/scaling/frame06.png"
#define KID_VJUMP_07 "dat/kid/scaling/frame07.png"
#define KID_VJUMP_08 "dat/kid/scaling/frame08.png"
#define KID_VJUMP_09 "dat/kid/scaling/frame09.png"
#define KID_VJUMP_10 "dat/kid/scaling/frame10.png"
#define KID_VJUMP_11 "dat/kid/scaling/frame11.png"
#define KID_VJUMP_12 "dat/kid/scaling/frame12.png"
#define KID_VJUMP_13 "dat/kid/scaling/frame13.png"
#define KID_VJUMP_15 "dat/kid/scaling/frame15.png"
#define KID_VJUMP_16 "dat/kid/scaling/frame16.png"
#define KID_VJUMP_17 "dat/kid/scaling/frame17.png"
#define KID_VJUMP_18 "dat/kid/scaling/frame18.png"
#define KID_VJUMP_19 "dat/kid/scaling/frame19.png"

#define KID_VJUMP_FRAMESET_NMEMB 18
extern struct frameset kid_vjump_frameset[KID_VJUMP_FRAMESET_NMEMB];

void load_kid_vjump (void);
void unload_kid_vjump (void);
void kid_vjump (struct anim *k);
bool is_kid_vjump (struct frame *f);

ALLEGRO_BITMAP *kid_vjump_01, *kid_vjump_02, *kid_vjump_03,
  *kid_vjump_04, *kid_vjump_05, *kid_vjump_06, *kid_vjump_07,
  *kid_vjump_08, *kid_vjump_09, *kid_vjump_10, *kid_vjump_11,
  *kid_vjump_12, *kid_vjump_13, *kid_vjump_15, *kid_vjump_16,
  *kid_vjump_17, *kid_vjump_18, *kid_vjump_19;

#endif	/* MININIM_KID_VJUMP_H */
