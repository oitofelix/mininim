/*
  guard-die.h -- guard die module;

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

#ifndef MININIM_GUARD_DIE_H
#define MININIM_GUARD_DIE_H

/* guard */
#define GUARD_DIE_01 "data/guard/die/01.png"
#define GUARD_DIE_02 "data/guard/die/02.png"
#define GUARD_DIE_03 "data/guard/die/03.png"
#define GUARD_DIE_04 "data/guard/die/04.png"
#define GUARD_DIE_05 "data/guard/die/05.png"
#define GUARD_DIE_06 "data/guard/die/06.png"
#define GUARD_DIE_SPIKED_00 "data/guard/die/spiked.png"
#define GUARD_DIE_CHOPPED_00 "data/guard/die/chopped.png"

/* fat guard */
#define FAT_GUARD_DIE_01 "data/fat-guard/die/01.png"
#define FAT_GUARD_DIE_02 "data/fat-guard/die/02.png"
#define FAT_GUARD_DIE_03 "data/fat-guard/die/03.png"
#define FAT_GUARD_DIE_04 "data/fat-guard/die/04.png"
#define FAT_GUARD_DIE_05 "data/fat-guard/die/05.png"
#define FAT_GUARD_DIE_06 "data/fat-guard/die/06.png"

#define GUARD_DIE_FRAMESET_NMEMB 6
extern struct frameset guard_die_frameset[GUARD_DIE_FRAMESET_NMEMB];
extern struct frameset fat_guard_die_frameset[GUARD_DIE_FRAMESET_NMEMB];

void load_guard_die (void);
void unload_guard_die (void);
void guard_die (struct anim *g);
void guard_die_spiked (struct anim *g);
void guard_die_chopped (struct anim *g);
void guard_die_suddenly (struct anim *g);
bool is_guard_dead (struct frame *f);
struct frameset *get_guard_die_frameset (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_die_01, *guard_die_02, *guard_die_03,
  *guard_die_04, *guard_die_05, *guard_die_06;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_die_01, *fat_guard_die_02, *fat_guard_die_03,
  *fat_guard_die_04, *fat_guard_die_05, *fat_guard_die_06;

#endif	/* MININIM_GUARD_DIE_H */
