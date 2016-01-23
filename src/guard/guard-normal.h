/*
  guard-normal.h -- guard normal module;

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

#ifndef MININIM_GUARD_NORMAL_H
#define MININIM_GUARD_NORMAL_H

/* guard */
#define GUARD_NORMAL_00 "data/guard/normal/00.png"

/* fat guard */
#define FAT_GUARD_NORMAL_00 "data/fat-guard/normal/00.png"

/* vizier */
#define VIZIER_NORMAL_00 "data/vizier/normal/00.png"

/* skeleton */
#define SKELETON_NORMAL_00 "data/skeleton/normal/00.png"

/* shadow */
#define SHADOW_NORMAL_00 "data/shadow/normal/00.png"

void load_guard_normal (void);
void unload_guard_normal (void);
void guard_normal (struct anim *g);
ALLEGRO_BITMAP *get_guard_normal_bitmap (enum anim_type t);

/* guard */
ALLEGRO_BITMAP *guard_normal_00;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_normal_00;

/* vizier */
ALLEGRO_BITMAP *vizier_normal_00;

/* skeleton */
ALLEGRO_BITMAP *skeleton_normal_00;

/* shadow */
ALLEGRO_BITMAP *shadow_normal_00;

#endif	/* MININIM_GUARD_NORMAL_H */
