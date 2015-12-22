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
#define KID_KEEP_SWORD_01 "dat/kid/got sword/frame05.png"
#define KID_KEEP_SWORD_02 "dat/kid/got sword/frame06.png"
#define KID_KEEP_SWORD_03 "dat/kid/got sword/frame07.png"
#define KID_KEEP_SWORD_04 "dat/kid/got sword/frame08.png"
#define KID_KEEP_SWORD_05 "dat/kid/putting down sword/frame01.png"
#define KID_KEEP_SWORD_06 "dat/kid/putting down sword/frame02.png"
#define KID_KEEP_SWORD_07 "dat/kid/putting down sword/frame03.png"
#define KID_KEEP_SWORD_08 "dat/kid/putting down sword/frame04.png"
#define KID_KEEP_SWORD_09 "dat/kid/putting down sword/frame05.png"
#define KID_KEEP_SWORD_10 "dat/kid/putting down sword/frame06.png"

#define KID_KEEP_SWORD_FRAMESET_NMEMB 10
extern struct frameset kid_keep_sword_frameset[KID_KEEP_SWORD_FRAMESET_NMEMB];

void load_kid_keep_sword (void);
void unload_kid_keep_sword (void);
void kid_keep_sword (struct anim *kid);

extern ALLEGRO_BITMAP *kid_keep_sword_01, *kid_keep_sword_02,
  *kid_keep_sword_03, *kid_keep_sword_04, *kid_keep_sword_05,
  *kid_keep_sword_06, *kid_keep_sword_07, *kid_keep_sword_08,
  *kid_keep_sword_09, *kid_keep_sword_10;

#endif	/* MININIM_KID_KEEP_SWORD_H */
