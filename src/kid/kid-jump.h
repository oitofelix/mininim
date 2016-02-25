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

#ifndef MININIM_KID_JUMP_H
#define MININIM_KID_JUMP_H

/* bitmaps */
#define KID_JUMP_00 "data/kid/jump/00.png"
#define KID_JUMP_01 "data/kid/jump/01.png"
#define KID_JUMP_02 "data/kid/jump/02.png"
#define KID_JUMP_03 "data/kid/jump/03.png"
#define KID_JUMP_04 "data/kid/jump/04.png"
#define KID_JUMP_05 "data/kid/jump/05.png"
#define KID_JUMP_06 "data/kid/jump/06.png"
#define KID_JUMP_07 "data/kid/jump/07.png"
#define KID_JUMP_08 "data/kid/jump/08.png"
#define KID_JUMP_09 "data/kid/jump/09.png"
#define KID_JUMP_10 "data/kid/jump/10.png"
#define KID_JUMP_11 "data/kid/jump/11.png"
#define KID_JUMP_12 "data/kid/jump/12.png"
#define KID_JUMP_13 "data/kid/jump/13.png"
#define KID_JUMP_14 "data/kid/jump/14.png"
#define KID_JUMP_15 "data/kid/jump/15.png"
#define KID_JUMP_16 "data/kid/jump/16.png"
#define KID_JUMP_17 "data/kid/jump/17.png"

#define KID_JUMP_FRAMESET_NMEMB 18
extern struct frameset kid_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

void load_kid_jump (void);
void unload_kid_jump (void);
void kid_jump (struct anim *k);
bool is_kid_jump (struct frame *f);
bool is_kid_jump_start (struct frame *f);
bool is_kid_jump_air (struct frame *f);
bool is_kid_jump_landing (struct frame *f);

extern ALLEGRO_BITMAP *kid_jump_00, *kid_jump_01, *kid_jump_02,
  *kid_jump_03, *kid_jump_04, *kid_jump_05, *kid_jump_06,
  *kid_jump_07, *kid_jump_08, *kid_jump_09, *kid_jump_10,
  *kid_jump_11, *kid_jump_12, *kid_jump_13, *kid_jump_14,
  *kid_jump_15, *kid_jump_16, *kid_jump_17;

#endif	/* MININIM_KID_JUMP_H */
