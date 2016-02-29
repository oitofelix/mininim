/*
  kid-hang.h -- kid hang module;

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

#ifndef MININIM_KID_HANG_H
#define MININIM_KID_HANG_H

/* bitmaps */
#define KID_HANG_00 "data/kid/hang/00.png"
#define KID_HANG_01 "data/kid/hang/01.png"
#define KID_HANG_02 "data/kid/hang/02.png"
#define KID_HANG_03 "data/kid/hang/03.png"
#define KID_HANG_04 "data/kid/hang/04.png"
#define KID_HANG_05 "data/kid/hang/05.png"
#define KID_HANG_06 "data/kid/hang/06.png"
#define KID_HANG_07 "data/kid/hang/07.png"
#define KID_HANG_08 "data/kid/hang/08.png"
#define KID_HANG_09 "data/kid/hang/09.png"
#define KID_HANG_10 "data/kid/hang/10.png"
#define KID_HANG_11 "data/kid/hang/11.png"
#define KID_HANG_12 "data/kid/hang/12.png"
#define KID_HANG_13 "data/kid/hang/13.png"

#define KID_HANG_FRAMESET_NMEMB 13
extern struct frameset kid_hang_frameset[KID_HANG_FRAMESET_NMEMB];

void load_kid_hang (void);
void unload_kid_hang (void);
void kid_hang (struct anim *k);
bool is_kid_hang (struct frame *f);
bool is_kid_hang_or_climb (struct frame *f);
bool is_kid_hanging_at_pos (struct frame *f, struct pos *p);

extern ALLEGRO_BITMAP *kid_hang_00, *kid_hang_01, *kid_hang_02,
  *kid_hang_03, *kid_hang_04, *kid_hang_05, *kid_hang_06,
  *kid_hang_07, *kid_hang_08, *kid_hang_09, *kid_hang_10,
  *kid_hang_11, *kid_hang_12, *kid_hang_13;

#endif	/* MININIM_KID_HANG_H */
