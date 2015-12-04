/*
  kid-walk.h -- kid walk module;

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

#ifndef FREEPOP_KID_WALK_H
#define FREEPOP_KID_WALK_H

/* bitmaps */
#define KID_WALK_01 "dat/kid/walking a step/frame01.png"
#define KID_WALK_02 "dat/kid/walking a step/frame02.png"
#define KID_WALK_03 "dat/kid/walking a step/frame03.png"
#define KID_WALK_04 "dat/kid/walking a step/frame04.png"
#define KID_WALK_05 "dat/kid/walking a step/frame05.png"
#define KID_WALK_06 "dat/kid/walking a step/frame06.png"
#define KID_WALK_07 "dat/kid/walking a step/frame07.png"
#define KID_WALK_08 "dat/kid/walking a step/frame08.png"
#define KID_WALK_09 "dat/kid/walking a step/frame09.png"
#define KID_WALK_10 "dat/kid/walking a step/frame10.png"
#define KID_WALK_11 "dat/kid/walking a step/frame11.png"
#define KID_WALK_12 "dat/kid/walking a step/frame12.png"

#define KID_WALK_FRAMESET_NMEMB 12
extern struct frameset kid_walk_frameset[KID_WALK_FRAMESET_NMEMB];

void load_kid_walk (void);
void unload_kid_walk (void);
void kid_walk (void);

extern ALLEGRO_BITMAP *kid_walk_01, *kid_walk_02, *kid_walk_03,
  *kid_walk_04, *kid_walk_05, *kid_walk_06, *kid_walk_07,
  *kid_walk_08, *kid_walk_09, *kid_walk_10, *kid_walk_11,
  *kid_walk_12;

#endif	/* FREEPOP_KID_WALK_H */
