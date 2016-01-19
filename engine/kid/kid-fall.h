/*
  kid-fall.h -- kid fall module;

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

#ifndef MININIM_KID_FALL_H
#define MININIM_KID_FALL_H

/* bitmaps */
#define KID_FALL_00 "data/kid/fall/00.png"
#define KID_FALL_01 "data/kid/fall/01.png"
#define KID_FALL_02 "data/kid/fall/02.png"
#define KID_FALL_03 "data/kid/fall/03.png"
#define KID_FALL_04 "data/kid/fall/04.png"

#define KID_FALL_FRAMESET_NMEMB 13
extern struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];

void load_kid_fall (void);
void unload_kid_fall (void);
void kid_fall (struct anim *k);
bool is_kid_fall (struct frame *f);

extern ALLEGRO_BITMAP *kid_fall_00, *kid_fall_01, *kid_fall_02,
  *kid_fall_03, *kid_fall_04;

#endif	/* MININIM_KID_FALL_H */
