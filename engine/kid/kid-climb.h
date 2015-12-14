/*
  kid-climb.h -- kid climb module;

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

#ifndef FREEPOP_KID_CLIMB_H
#define FREEPOP_KID_CLIMB_H

/* bitmaps */
#define KID_CLIMB_01 "dat/kid/clipping/frame01.png"
#define KID_CLIMB_02 "dat/kid/clipping/frame02.png"
#define KID_CLIMB_03 "dat/kid/clipping/frame03.png"
#define KID_CLIMB_04 "dat/kid/clipping/frame04.png"
#define KID_CLIMB_05 "dat/kid/clipping/frame05.png"
#define KID_CLIMB_06 "dat/kid/clipping/frame06.png"
#define KID_CLIMB_07 "dat/kid/clipping/frame07.png"
#define KID_CLIMB_08 "dat/kid/clipping/frame08.png"
#define KID_CLIMB_09 "dat/kid/clipping/frame09.png"
#define KID_CLIMB_10 "dat/kid/clipping/frame10.png"
#define KID_CLIMB_11 "dat/kid/clipping/frame11.png"
#define KID_CLIMB_12 "dat/kid/clipping/frame12.png"
#define KID_CLIMB_13 "dat/kid/clipping/frame13.png"
#define KID_CLIMB_14 "dat/kid/clipping/frame14.png"
#define KID_CLIMB_15 "dat/kid/clipping/frame15.png"

#define KID_CLIMB_FRAMESET_NMEMB 15
extern struct frameset kid_climb_frameset[KID_CLIMB_FRAMESET_NMEMB];

void load_kid_climb (void);
void unload_kid_climb (void);
void kid_climb (struct anim *kid);
bool is_kid_climb (struct frame *f);
bool is_kid_successfully_climbing_at_pos (struct frame *f, struct pos *hang_pos,
                                          struct pos *p);
bool is_kid_successfully_climbing (struct frame *f);

extern ALLEGRO_BITMAP *kid_climb_01, *kid_climb_02, *kid_climb_03,
  *kid_climb_04, *kid_climb_05, *kid_climb_06, *kid_climb_07,
  *kid_climb_08, *kid_climb_09, *kid_climb_10, *kid_climb_11,
  *kid_climb_12, *kid_climb_13, *kid_climb_14, *kid_climb_15;

#endif	/* FREEPOP_KID_CLIMB_H */
