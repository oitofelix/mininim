/*
  guard-walkb.h -- guard walk backward module;

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

#ifndef MININIM_GUARD_WALKB_H
#define MININIM_GUARD_WALKB_H

/* guard */
#define GUARD_WALKB_01 "data/guard/walkb/01.png"
#define GUARD_WALKB_02 "data/guard/walkb/02.png"

/* fat guard */
#define FAT_GUARD_WALKB_01 "data/fat-guard/walkb/01.png"
#define FAT_GUARD_WALKB_02 "data/fat-guard/walkb/02.png"

/* vizier */
#define VIZIER_WALKB_01 "data/vizier/walkb/01.png"
#define VIZIER_WALKB_02 "data/vizier/walkb/02.png"

/* skeleton */
#define SKELETON_WALKB_01 "data/skeleton/walkb/01.png"
#define SKELETON_WALKB_02 "data/skeleton/walkb/02.png"

/* shadow */
#define SHADOW_WALKB_01 "data/shadow/walkb/01.png"
#define SHADOW_WALKB_02 "data/shadow/walkb/02.png"

#define GUARD_WALKB_FRAMESET_NMEMB 2
extern struct frameset guard_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
extern struct frameset fat_guard_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
extern struct frameset vizier_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
extern struct frameset skeleton_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];
extern struct frameset shadow_walkb_frameset[GUARD_WALKB_FRAMESET_NMEMB];

void load_guard_walkb (void);
void unload_guard_walkb (void);
void guard_walkb (struct anim *k);
struct frameset *get_guard_walkb_frameset (enum anim_type t);

/* guard */
ALLEGRO_BITMAP *guard_walkb_01, *guard_walkb_02;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_walkb_01, *fat_guard_walkb_02;

/* vizier */
ALLEGRO_BITMAP *vizier_walkb_01, *vizier_walkb_02;

/* skeleton */
ALLEGRO_BITMAP *skeleton_walkb_01, *skeleton_walkb_02;

/* shadow */
ALLEGRO_BITMAP *shadow_walkb_01, *shadow_walkb_02;

#endif	/* MININIM_GUARD_WALKB_H */
