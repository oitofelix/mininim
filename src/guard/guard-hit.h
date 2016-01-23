/*
  guard-hit.h -- guard hit module;

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

#ifndef MININIM_GUARD_HIT_H
#define MININIM_GUARD_HIT_H

/* guard */
#define GUARD_HIT_00 "data/guard/hit/00.png"
#define GUARD_HIT_01 "data/guard/hit/01.png"
#define GUARD_HIT_02 "data/guard/hit/02.png"
#define GUARD_HIT_03 "data/guard/hit/03.png"
#define GUARD_HIT_04 "data/guard/hit/04.png"

/* fat guard */
#define FAT_GUARD_HIT_00 "data/fat-guard/hit/00.png"
#define FAT_GUARD_HIT_01 "data/fat-guard/hit/01.png"
#define FAT_GUARD_HIT_02 "data/fat-guard/hit/02.png"
#define FAT_GUARD_HIT_03 "data/fat-guard/hit/03.png"
#define FAT_GUARD_HIT_04 "data/fat-guard/hit/04.png"

/* vizier */
#define VIZIER_HIT_00 "data/vizier/hit/00.png"
#define VIZIER_HIT_01 "data/vizier/hit/01.png"
#define VIZIER_HIT_02 "data/vizier/hit/02.png"
#define VIZIER_HIT_03 "data/vizier/hit/03.png"
#define VIZIER_HIT_04 "data/vizier/hit/04.png"

/* skeleton */
#define SKELETON_HIT_00 "data/skeleton/hit/00.png"
#define SKELETON_HIT_01 "data/skeleton/hit/01.png"
#define SKELETON_HIT_02 "data/skeleton/hit/02.png"
#define SKELETON_HIT_03 "data/skeleton/hit/03.png"
#define SKELETON_HIT_04 "data/skeleton/hit/04.png"

/* shadow */
#define SHADOW_HIT_00 "data/shadow/hit/00.png"
#define SHADOW_HIT_01 "data/shadow/hit/01.png"
#define SHADOW_HIT_02 "data/shadow/hit/02.png"
#define SHADOW_HIT_03 "data/shadow/hit/03.png"
#define SHADOW_HIT_04 "data/shadow/hit/04.png"

#define GUARD_HIT_FRAMESET_NMEMB 5
extern struct frameset guard_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
extern struct frameset fat_guard_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
extern struct frameset vizier_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
extern struct frameset skeleton_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];
extern struct frameset shadow_hit_frameset[GUARD_HIT_FRAMESET_NMEMB];

void load_guard_hit (void);
void unload_guard_hit (void);
void guard_hit (struct anim *k);
struct frameset *get_guard_hit_frameset (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_hit_00, *guard_hit_01,
  *guard_hit_02, *guard_hit_03, *guard_hit_04;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_hit_00, *fat_guard_hit_01,
  *fat_guard_hit_02, *fat_guard_hit_03, *fat_guard_hit_04;

/* vizier */
extern ALLEGRO_BITMAP *vizier_hit_00, *vizier_hit_01,
  *vizier_hit_02, *vizier_hit_03, *vizier_hit_04;

/* skeleton */
extern ALLEGRO_BITMAP *skeleton_hit_00, *skeleton_hit_01,
  *skeleton_hit_02, *skeleton_hit_03, *skeleton_hit_04;

/* shadow */
extern ALLEGRO_BITMAP *shadow_hit_00, *shadow_hit_01,
  *shadow_hit_02, *shadow_hit_03, *shadow_hit_04;

#endif	/* MININIM_GUARD_HIT_H */
