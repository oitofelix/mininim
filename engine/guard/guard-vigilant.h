/*
  guard-vigilant.h -- guard vigilant module;

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

#ifndef MININIM_GUARD_VIGILANT_H
#define MININIM_GUARD_VIGILANT_H

/* guard */
#define GUARD_VIGILANT_01 "data/guard/vigilant/01.png"
#define GUARD_VIGILANT_02 "data/guard/vigilant/02.png"
#define GUARD_VIGILANT_03 "data/guard/vigilant/03.png"

/* fat guard */
#define FAT_GUARD_VIGILANT_01 "data/fat-guard/vigilant/01.png"
#define FAT_GUARD_VIGILANT_02 "data/fat-guard/vigilant/02.png"
#define FAT_GUARD_VIGILANT_03 "data/fat-guard/vigilant/03.png"

#define GUARD_VIGILANT_FRAMESET_NMEMB 3
extern struct frameset guard_vigilant_frameset[GUARD_VIGILANT_FRAMESET_NMEMB];
extern struct frameset fat_guard_vigilant_frameset[GUARD_VIGILANT_FRAMESET_NMEMB];

void load_guard_vigilant (void);
void unload_guard_vigilant (void);
void guard_vigilant (struct anim *k);
struct frameset *get_guard_vigilant_frameset (enum anim_type t);

extern ALLEGRO_BITMAP *guard_vigilant_01, *guard_vigilant_02,
  *guard_vigilant_03;

extern ALLEGRO_BITMAP *fat_guard_vigilant_01, *fat_guard_vigilant_02,
  *fat_guard_vigilant_03;

#endif	/* MININIM_GUARD_VIGILANT_H */
