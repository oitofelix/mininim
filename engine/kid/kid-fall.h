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

#ifndef FREEPOP_KID_FALL_H
#define FREEPOP_KID_FALL_H

/* bitmaps */
#define KID_FALL_13 "dat/kid/hanging and falling/frame13.png"
#define KID_FALL_14 "dat/kid/hanging and falling/frame14.png"
#define KID_FALL_15 "dat/kid/hanging and falling/frame15.png"
#define KID_FALL_16 "dat/kid/hanging and falling/frame16.png"
#define KID_FALL_17 "dat/kid/hanging and falling/frame17.png"

#define KID_FALL_FRAMESET_NMEMB 13
extern struct frameset kid_fall_frameset[KID_FALL_FRAMESET_NMEMB];

void load_kid_fall (void);
void unload_kid_fall (void);
void kid_fall (struct anim *kid);
bool is_kid_fall (struct frame *f);

extern ALLEGRO_BITMAP *kid_fall_13, *kid_fall_14, *kid_fall_15,
  *kid_fall_16, *kid_fall_17;

#endif	/* FREEPOP_KID_FALL_H */
