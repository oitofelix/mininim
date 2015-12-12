/*
  kid-jump.h -- kid jump module;

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

#ifndef FREEPOP_KID_JUMP_H
#define FREEPOP_KID_JUMP_H

/* bitmaps */
#define KID_JUMP_01 "dat/kid/simple jump/frame01.png"
#define KID_JUMP_02 "dat/kid/simple jump/frame02.png"
#define KID_JUMP_03 "dat/kid/simple jump/frame03.png"
#define KID_JUMP_04 "dat/kid/simple jump/frame04.png"
#define KID_JUMP_05 "dat/kid/simple jump/frame05.png"
#define KID_JUMP_06 "dat/kid/simple jump/frame06.png"
#define KID_JUMP_07 "dat/kid/simple jump/frame07.png"
#define KID_JUMP_08 "dat/kid/simple jump/frame08.png"
#define KID_JUMP_09 "dat/kid/simple jump/frame09.png"
#define KID_JUMP_10 "dat/kid/simple jump/frame10.png"
#define KID_JUMP_11 "dat/kid/simple jump/frame11.png"
#define KID_JUMP_12 "dat/kid/simple jump/frame12.png"
#define KID_JUMP_13 "dat/kid/simple jump/frame13.png"
#define KID_JUMP_14 "dat/kid/simple jump/frame14.png"
#define KID_JUMP_15 "dat/kid/simple jump/frame15.png"
#define KID_JUMP_16 "dat/kid/simple jump/frame16.png"
#define KID_JUMP_17 "dat/kid/simple jump/frame17.png"
#define KID_JUMP_18 "dat/kid/simple jump/frame18.png"

#define KID_JUMP_FRAMESET_NMEMB 18
extern struct frameset kid_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

void load_kid_jump (void);
void unload_kid_jump (void);
void kid_jump (struct anim *kid);

extern ALLEGRO_BITMAP *kid_jump_01, *kid_jump_02, *kid_jump_03,
  *kid_jump_04, *kid_jump_05, *kid_jump_06, *kid_jump_07,
  *kid_jump_08, *kid_jump_09, *kid_jump_10, *kid_jump_11,
  *kid_jump_12, *kid_jump_13, *kid_jump_14, *kid_jump_15,
  *kid_jump_16, *kid_jump_17, *kid_jump_18;

#endif	/* FREEPOP_KID_JUMP_H */
