/*
  guard-walkf.h -- guard walk forward module;

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

#ifndef MININIM_GUARD_WALKF_H
#define MININIM_GUARD_WALKF_H

/* guard */
#define GUARD_WALKF_00 "data/guard/walkf/00.png"
#define GUARD_WALKF_01 "data/guard/walkf/01.png"
#define GUARD_WALKF_02 "data/guard/walkf/02.png"

/* fat guard */
#define FAT_GUARD_WALKF_00 "data/fat-guard/walkf/00.png"
#define FAT_GUARD_WALKF_01 "data/fat-guard/walkf/01.png"
#define FAT_GUARD_WALKF_02 "data/fat-guard/walkf/02.png"

/* vizier */
#define VIZIER_WALKF_00 "data/vizier/walkf/00.png"
#define VIZIER_WALKF_01 "data/vizier/walkf/01.png"
#define VIZIER_WALKF_02 "data/vizier/walkf/02.png"

/* skeleton */
#define SKELETON_WALKF_00 "data/skeleton/walkf/00.png"
#define SKELETON_WALKF_01 "data/skeleton/walkf/01.png"
#define SKELETON_WALKF_02 "data/skeleton/walkf/02.png"

/* shadow */
#define SHADOW_WALKF_00 "data/shadow/walkf/00.png"
#define SHADOW_WALKF_01 "data/shadow/walkf/01.png"
#define SHADOW_WALKF_02 "data/shadow/walkf/02.png"

#define GUARD_WALKF_FRAMESET_NMEMB 3
extern struct frameset guard_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
extern struct frameset fat_guard_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
extern struct frameset vizier_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
extern struct frameset skeleton_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];
extern struct frameset shadow_walkf_frameset[GUARD_WALKF_FRAMESET_NMEMB];

void load_guard_walkf (void);
void unload_guard_walkf (void);
void guard_walkf (struct anim *g);
struct frameset *get_guard_walkf_frameset (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_walkf_00, *guard_walkf_01, *guard_walkf_02;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_walkf_00, *fat_guard_walkf_01, *fat_guard_walkf_02;

/* vizier */
extern ALLEGRO_BITMAP *vizier_walkf_00, *vizier_walkf_01, *vizier_walkf_02;

/* skeleton */
extern ALLEGRO_BITMAP *skeleton_walkf_00, *skeleton_walkf_01, *skeleton_walkf_02;

/* shadow */
extern ALLEGRO_BITMAP *shadow_walkf_00, *shadow_walkf_01, *shadow_walkf_02;

#endif	/* MININIM_GUARD_WALKF_H */
