/*
  guard-fall.h -- guard fall module;

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

#ifndef MININIM_GUARD_FALL_H
#define MININIM_GUARD_FALL_H

/* guard */
#define GUARD_FALL_00 "data/guard/fall/00.png"
#define GUARD_FALL_01 "data/guard/fall/01.png"
#define GUARD_FALL_02 "data/guard/fall/02.png"

/* fat guard */
#define FAT_GUARD_FALL_00 "data/fat-guard/fall/00.png"
#define FAT_GUARD_FALL_01 "data/fat-guard/fall/01.png"
#define FAT_GUARD_FALL_02 "data/fat-guard/fall/02.png"

/* vizier */
#define VIZIER_FALL_00 "data/vizier/fall/00.png"
#define VIZIER_FALL_01 "data/vizier/fall/01.png"
#define VIZIER_FALL_02 "data/vizier/fall/02.png"

/* skeleton */
#define SKELETON_FALL_00 "data/skeleton/fall/00.png"
#define SKELETON_FALL_01 "data/skeleton/fall/01.png"
#define SKELETON_FALL_02 "data/skeleton/fall/02.png"

/* shadow */
#define SHADOW_FALL_00 "data/shadow/fall/00.png"
#define SHADOW_FALL_01 "data/shadow/fall/01.png"
#define SHADOW_FALL_02 "data/shadow/fall/02.png"

#define GUARD_FALL_FRAMESET_NMEMB 3
extern struct frameset guard_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];
extern struct frameset fat_guard_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];
extern struct frameset vizier_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];
extern struct frameset skeleton_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];
extern struct frameset shadow_fall_frameset[GUARD_FALL_FRAMESET_NMEMB];

void load_guard_fall (void);
void unload_guard_fall (void);
void guard_fall (struct anim *g);
bool is_guard_fall (struct frame *f);
struct frameset *get_guard_fall_frameset (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_fall_00, *guard_fall_01, *guard_fall_02;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_fall_00, *fat_guard_fall_01, *fat_guard_fall_02;

/* vizier */
extern ALLEGRO_BITMAP *vizier_fall_00, *vizier_fall_01, *vizier_fall_02;

/* skeleton */
extern ALLEGRO_BITMAP *skeleton_fall_00, *skeleton_fall_01, *skeleton_fall_02;

/* shadow */
extern ALLEGRO_BITMAP *shadow_fall_00, *shadow_fall_01, *shadow_fall_02;

#endif	/* MININIM_GUARD_FALL_H */
