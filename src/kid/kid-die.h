/*
  kid-die.h -- kid die module;

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

#ifndef MININIM_KID_DIE_H
#define MININIM_KID_DIE_H

/* bitmaps */
#define KID_DIE_00 "data/kid/die/00.png"
#define KID_DIE_01 "data/kid/die/01.png"
#define KID_DIE_02 "data/kid/die/02.png"
#define KID_DIE_03 "data/kid/die/03.png"
#define KID_DIE_04 "data/kid/die/04.png"
#define KID_DIE_05 "data/kid/die/05.png"
#define KID_DIE_SPIKED_00 "data/kid/die/spiked.png"
#define KID_DIE_CHOPPED_00 "data/kid/die/chopped.png"

#define KID_DIE_FRAMESET_NMEMB 6
extern struct frameset kid_die_frameset[KID_DIE_FRAMESET_NMEMB];

void load_kid_die (void);
void unload_kid_die (void);
void kid_resurrect (struct anim *k);
void kid_die (struct anim *k);
void kid_die_spiked (struct anim *k);
void kid_die_chopped (struct anim *k);
void kid_die_suddenly (struct anim *k);
bool is_kid_dead (struct frame *f);
bool is_kid_chopped (struct frame *f);
void kill_kid_shadows (struct anim *k);

extern ALLEGRO_BITMAP *kid_die_00, *kid_die_01, *kid_die_02,
  *kid_die_03, *kid_die_04, *kid_die_05;

#endif	/* MININIM_KID_DIE_H */
