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

#ifndef MININIM_KID_WALK_H
#define MININIM_KID_WALK_H

/* bitmaps */
#define KID_WALK_00 "data/kid/walk/00.png"
#define KID_WALK_01 "data/kid/walk/01.png"
#define KID_WALK_02 "data/kid/walk/02.png"
#define KID_WALK_03 "data/kid/walk/03.png"
#define KID_WALK_04 "data/kid/walk/04.png"
#define KID_WALK_05 "data/kid/walk/05.png"
#define KID_WALK_06 "data/kid/walk/06.png"
#define KID_WALK_07 "data/kid/walk/07.png"
#define KID_WALK_08 "data/kid/walk/08.png"
#define KID_WALK_09 "data/kid/walk/09.png"
#define KID_WALK_10 "data/kid/walk/10.png"
#define KID_WALK_11 "data/kid/walk/11.png"

#define KID_WALK_FRAMESET_NMEMB 12
extern struct frameset kid_walk_frameset[KID_WALK_FRAMESET_NMEMB];

void load_kid_walk (void);
void unload_kid_walk (void);
void kid_walk (struct anim *k);

extern ALLEGRO_BITMAP *kid_walk_00, *kid_walk_01, *kid_walk_02,
  *kid_walk_03, *kid_walk_04, *kid_walk_05, *kid_walk_06,
  *kid_walk_07, *kid_walk_08, *kid_walk_09, *kid_walk_10,
  *kid_walk_11;

#endif	/* MININIM_KID_WALK_H */
