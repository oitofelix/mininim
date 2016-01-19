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

#ifndef MININIM_KID_CLIMB_H
#define MININIM_KID_CLIMB_H

/* bitmaps */
#define KID_CLIMB_00 "data/kid/climb/00.png"
#define KID_CLIMB_01 "data/kid/climb/01.png"
#define KID_CLIMB_02 "data/kid/climb/02.png"
#define KID_CLIMB_03 "data/kid/climb/03.png"
#define KID_CLIMB_04 "data/kid/climb/04.png"
#define KID_CLIMB_05 "data/kid/climb/05.png"
#define KID_CLIMB_06 "data/kid/climb/06.png"
#define KID_CLIMB_07 "data/kid/climb/07.png"
#define KID_CLIMB_08 "data/kid/climb/08.png"
#define KID_CLIMB_09 "data/kid/climb/09.png"
#define KID_CLIMB_10 "data/kid/climb/10.png"
#define KID_CLIMB_11 "data/kid/climb/11.png"
#define KID_CLIMB_12 "data/kid/climb/12.png"
#define KID_CLIMB_13 "data/kid/climb/13.png"
#define KID_CLIMB_14 "data/kid/climb/14.png"

#define KID_CLIMB_FRAMESET_NMEMB 15
extern struct frameset kid_climb_frameset[KID_CLIMB_FRAMESET_NMEMB];

void load_kid_climb (void);
void unload_kid_climb (void);
void kid_climb (struct anim *k);
bool is_kid_climb (struct frame *f);
bool is_kid_successfully_climbing_at_pos (struct frame *f, struct pos *hang_pos,
                                          struct pos *p);
bool is_kid_successfully_climbing (struct frame *f);

extern ALLEGRO_BITMAP *kid_climb_00, *kid_climb_01, *kid_climb_02,
  *kid_climb_03, *kid_climb_04, *kid_climb_05, *kid_climb_06,
  *kid_climb_07, *kid_climb_08, *kid_climb_09, *kid_climb_10,
  *kid_climb_11, *kid_climb_12, *kid_climb_13, *kid_climb_14;

#endif	/* MININIM_KID_CLIMB_H */
