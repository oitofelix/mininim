/*
  kid-take-sword.h -- kid take sword module;

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

#ifndef MININIM_KID_TAKE_SWORD_H
#define MININIM_KID_TAKE_SWORD_H

/* bitmaps */
#define KID_TAKE_SWORD_00 "data/kid/take-sword/00.png"
#define KID_TAKE_SWORD_01 "data/kid/take-sword/01.png"
#define KID_TAKE_SWORD_02 "data/kid/take-sword/02.png"
#define KID_TAKE_SWORD_03 "data/kid/take-sword/03.png"

#define KID_TAKE_SWORD_FRAMESET_NMEMB 4
extern struct frameset kid_take_sword_frameset[KID_TAKE_SWORD_FRAMESET_NMEMB];

void load_kid_take_sword (void);
void unload_kid_take_sword (void);
void kid_take_sword (struct anim *k);

extern ALLEGRO_BITMAP *kid_take_sword_00, *kid_take_sword_01,
  *kid_take_sword_02, *kid_take_sword_03;

#endif	/* MININIM_KID_TAKE_SWORD_H */
