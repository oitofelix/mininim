/*
  kid-stairs.h -- kid stairs module;

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

#ifndef FREEPOP_KID_STAIRS_H
#define FREEPOP_KID_STAIRS_H

/* bitmaps */
#define KID_STAIRS_01 "dat/kid/stairs/frame01.png"
#define KID_STAIRS_02 "dat/kid/stairs/frame02.png"
#define KID_STAIRS_03 "dat/kid/stairs/frame03.png"
#define KID_STAIRS_04 "dat/kid/stairs/frame04.png"
#define KID_STAIRS_05 "dat/kid/stairs/frame05.png"
#define KID_STAIRS_06 "dat/kid/stairs/frame06.png"
#define KID_STAIRS_07 "dat/kid/stairs/frame07.png"
#define KID_STAIRS_08 "dat/kid/stairs/frame08.png"
#define KID_STAIRS_09 "dat/kid/stairs/frame09.png"
#define KID_STAIRS_10 "dat/kid/stairs/frame10.png"
#define KID_STAIRS_11 "dat/kid/stairs/frame11.png"
#define KID_STAIRS_12 "dat/kid/stairs/frame12.png"

#define KID_STAIRS_FRAMESET_NMEMB 12
extern struct frameset kid_stairs_frameset[KID_STAIRS_FRAMESET_NMEMB];

void load_kid_stairs (void);
void unload_kid_stairs (void);
void kid_stairs (struct anim *kid);
bool is_kid_stairs (struct frame *f);

extern ALLEGRO_BITMAP *kid_stairs_01, *kid_stairs_02, *kid_stairs_03,
  *kid_stairs_04, *kid_stairs_05, *kid_stairs_06, *kid_stairs_07,
  *kid_stairs_08, *kid_stairs_09, *kid_stairs_10, *kid_stairs_11,
  *kid_stairs_12;

#endif	/* FREEPOP_KID_STAIRS_H */
