/*
  kid-couch.h -- kid couch module;

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

#ifndef FREEPOP_KID_COUCH_H
#define FREEPOP_KID_COUCH_H

/* bitmaps */
#define KID_COUCH_01 "dat/kid/couching/frame01.png"
#define KID_COUCH_02 "dat/kid/couching/frame02.png"
#define KID_COUCH_03 "dat/kid/couching/frame03.png"
#define KID_COUCH_04 "dat/kid/couching/frame04.png"
#define KID_COUCH_05 "dat/kid/couching/frame05.png"
#define KID_COUCH_06 "dat/kid/couching/frame06.png"
#define KID_COUCH_07 "dat/kid/couching/frame07.png"
#define KID_COUCH_08 "dat/kid/couching/frame08.png"
#define KID_COUCH_09 "dat/kid/couching/frame09.png"
#define KID_COUCH_10 "dat/kid/couching/frame10.png"
#define KID_COUCH_11 "dat/kid/couching/frame11.png"
#define KID_COUCH_12 "dat/kid/couching/frame12.png"
#define KID_COUCH_13 "dat/kid/couching/frame13.png"

#define KID_COUCH_FRAMESET_NMEMB 13
extern struct frameset kid_couch_frameset[KID_COUCH_FRAMESET_NMEMB];

void load_kid_couch (void);
void unload_kid_couch (void);
void kid_couch (struct anim *kid);
void kid_couch_collision (struct anim *kid);
void kid_couch_suddenly (struct anim *kid);
bool is_kid_couch (struct frame *f);

extern ALLEGRO_BITMAP *kid_couch_01, *kid_couch_02, *kid_couch_03, *kid_couch_04,
  *kid_couch_05, *kid_couch_06, *kid_couch_07, *kid_couch_08,
  *kid_couch_09, *kid_couch_10, *kid_couch_11, *kid_couch_12,
  *kid_couch_13;

#endif	/* FREEPOP_KID_COUCH_H */
