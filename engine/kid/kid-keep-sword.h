/*
  kid-keep-sword.h -- kid keep sword module;

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

#ifndef MININIM_KID_KEEP_SWORD_H
#define MININIM_KID_KEEP_SWORD_H

/* bitmaps */
#define KID_KEEP_SWORD_00 "data/kid/keep-sword/00.png"
#define KID_KEEP_SWORD_01 "data/kid/keep-sword/01.png"
#define KID_KEEP_SWORD_02 "data/kid/keep-sword/02.png"
#define KID_KEEP_SWORD_03 "data/kid/keep-sword/03.png"
#define KID_KEEP_SWORD_04 "data/kid/keep-sword/04.png"
#define KID_KEEP_SWORD_05 "data/kid/keep-sword/05.png"
#define KID_KEEP_SWORD_06 "data/kid/keep-sword/06.png"
#define KID_KEEP_SWORD_07 "data/kid/keep-sword/07.png"
#define KID_KEEP_SWORD_08 "data/kid/keep-sword/08.png"
#define KID_KEEP_SWORD_09 "data/kid/keep-sword/09.png"

#define KID_KEEP_SWORD_FRAMESET_NMEMB 10
extern struct frameset kid_keep_sword_frameset[KID_KEEP_SWORD_FRAMESET_NMEMB];

void load_kid_keep_sword (void);
void unload_kid_keep_sword (void);
void kid_keep_sword (struct anim *k);

extern ALLEGRO_BITMAP *kid_keep_sword_00, *kid_keep_sword_01,
  *kid_keep_sword_02, *kid_keep_sword_03, *kid_keep_sword_04,
  *kid_keep_sword_05, *kid_keep_sword_06, *kid_keep_sword_07,
  *kid_keep_sword_08, *kid_keep_sword_09;

#endif	/* MININIM_KID_KEEP_SWORD_H */
