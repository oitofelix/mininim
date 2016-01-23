/*
  guard-attack.h -- guard attack module;

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

#ifndef MININIM_GUARD_ATTACK_H
#define MININIM_GUARD_ATTACK_H

/* guard */
#define GUARD_ATTACK_00 "data/guard/attack/00.png"
#define GUARD_ATTACK_01 "data/guard/attack/01.png"
#define GUARD_ATTACK_02 "data/guard/attack/02.png"
#define GUARD_ATTACK_03 "data/guard/attack/03.png"
#define GUARD_ATTACK_04 "data/guard/attack/04.png"
#define GUARD_ATTACK_05 "data/guard/attack/05.png"
#define GUARD_ATTACK_06 "data/guard/attack/06.png"
#define GUARD_ATTACK_07 "data/guard/attack/07.png"
#define GUARD_ATTACK_COUNTER_DEFENSE_00 "data/guard/attack/counter-defense-00.png"
#define GUARD_ATTACK_COUNTER_DEFENSE_01 "data/guard/attack/counter-defense-01.png"
#define GUARD_ATTACK_DEFENDED "data/guard/attack/defended.png"

/* fat guard */
#define FAT_GUARD_ATTACK_00 "data/fat-guard/attack/00.png"
#define FAT_GUARD_ATTACK_01 "data/fat-guard/attack/01.png"
#define FAT_GUARD_ATTACK_02 "data/fat-guard/attack/02.png"
#define FAT_GUARD_ATTACK_03 "data/fat-guard/attack/03.png"
#define FAT_GUARD_ATTACK_04 "data/fat-guard/attack/04.png"
#define FAT_GUARD_ATTACK_05 "data/fat-guard/attack/05.png"
#define FAT_GUARD_ATTACK_06 "data/fat-guard/attack/06.png"
#define FAT_GUARD_ATTACK_07 "data/fat-guard/attack/07.png"
#define FAT_GUARD_ATTACK_COUNTER_DEFENSE_00 "data/fat-guard/attack/counter-defense-00.png"
#define FAT_GUARD_ATTACK_COUNTER_DEFENSE_01 "data/fat-guard/attack/counter-defense-01.png"
#define FAT_GUARD_ATTACK_DEFENDED "data/fat-guard/attack/defended.png"

/* vizier */
#define VIZIER_ATTACK_00 "data/vizier/attack/00.png"
#define VIZIER_ATTACK_01 "data/vizier/attack/01.png"
#define VIZIER_ATTACK_02 "data/vizier/attack/02.png"
#define VIZIER_ATTACK_03 "data/vizier/attack/03.png"
#define VIZIER_ATTACK_04 "data/vizier/attack/04.png"
#define VIZIER_ATTACK_05 "data/vizier/attack/05.png"
#define VIZIER_ATTACK_06 "data/vizier/attack/06.png"
#define VIZIER_ATTACK_07 "data/vizier/attack/07.png"
#define VIZIER_ATTACK_COUNTER_DEFENSE_00 "data/vizier/attack/counter-defense-00.png"
#define VIZIER_ATTACK_COUNTER_DEFENSE_01 "data/vizier/attack/counter-defense-01.png"
#define VIZIER_ATTACK_DEFENDED "data/vizier/attack/defended.png"

/* skeleton */
#define SKELETON_ATTACK_00 "data/skeleton/attack/00.png"
#define SKELETON_ATTACK_01 "data/skeleton/attack/01.png"
#define SKELETON_ATTACK_02 "data/skeleton/attack/02.png"
#define SKELETON_ATTACK_03 "data/skeleton/attack/03.png"
#define SKELETON_ATTACK_04 "data/skeleton/attack/04.png"
#define SKELETON_ATTACK_05 "data/skeleton/attack/05.png"
#define SKELETON_ATTACK_06 "data/skeleton/attack/06.png"
#define SKELETON_ATTACK_07 "data/skeleton/attack/07.png"
#define SKELETON_ATTACK_COUNTER_DEFENSE_00 "data/skeleton/attack/counter-defense-00.png"
#define SKELETON_ATTACK_COUNTER_DEFENSE_01 "data/skeleton/attack/counter-defense-01.png"
#define SKELETON_ATTACK_DEFENDED "data/skeleton/attack/defended.png"

/* shadow */
#define SHADOW_ATTACK_00 "data/shadow/attack/00.png"
#define SHADOW_ATTACK_01 "data/shadow/attack/01.png"
#define SHADOW_ATTACK_02 "data/shadow/attack/02.png"
#define SHADOW_ATTACK_03 "data/shadow/attack/03.png"
#define SHADOW_ATTACK_04 "data/shadow/attack/04.png"
#define SHADOW_ATTACK_05 "data/shadow/attack/05.png"
#define SHADOW_ATTACK_06 "data/shadow/attack/06.png"
#define SHADOW_ATTACK_07 "data/shadow/attack/07.png"
#define SHADOW_ATTACK_COUNTER_DEFENSE_00 "data/shadow/attack/counter-defense-00.png"
#define SHADOW_ATTACK_COUNTER_DEFENSE_01 "data/shadow/attack/counter-defense-01.png"
#define SHADOW_ATTACK_DEFENDED "data/shadow/attack/defended.png"

#define GUARD_ATTACK_FRAMESET_NMEMB 10
extern struct frameset guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
extern struct frameset fat_guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
extern struct frameset vizier_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
extern struct frameset skeleton_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
extern struct frameset shadow_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];

void load_guard_attack (void);
void unload_guard_attack (void);
void guard_attack (struct anim *g);
struct frameset *get_guard_attack_frameset (enum anim_type t);
ALLEGRO_BITMAP *get_guard_attack_defended_bitmap (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_attack_00, *guard_attack_01,
  *guard_attack_02, *guard_attack_03, *guard_attack_04,
  *guard_attack_05, *guard_attack_06, *guard_attack_07,
  *guard_attack_counter_defense_00, *guard_attack_counter_defense_01,
  *guard_attack_defended;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_attack_00, *fat_guard_attack_01,
  *fat_guard_attack_02, *fat_guard_attack_03, *fat_guard_attack_04,
  *fat_guard_attack_05, *fat_guard_attack_06, *fat_guard_attack_07,
  *fat_guard_attack_counter_defense_00, *fat_guard_attack_counter_defense_01,
  *fat_guard_attack_defended;

/* vizier */
extern ALLEGRO_BITMAP *vizier_attack_00, *vizier_attack_01,
  *vizier_attack_02, *vizier_attack_03, *vizier_attack_04,
  *vizier_attack_05, *vizier_attack_06, *vizier_attack_07,
  *vizier_attack_counter_defense_00, *vizier_attack_counter_defense_01,
  *vizier_attack_defended;

/* skeleton */
extern ALLEGRO_BITMAP *skeleton_attack_00, *skeleton_attack_01,
  *skeleton_attack_02, *skeleton_attack_03, *skeleton_attack_04,
  *skeleton_attack_05, *skeleton_attack_06, *skeleton_attack_07,
  *skeleton_attack_counter_defense_00, *skeleton_attack_counter_defense_01,
  *skeleton_attack_defended;

/* shadow */
extern ALLEGRO_BITMAP *shadow_attack_00, *shadow_attack_01,
  *shadow_attack_02, *shadow_attack_03, *shadow_attack_04,
  *shadow_attack_05, *shadow_attack_06, *shadow_attack_07,
  *shadow_attack_counter_defense_00, *shadow_attack_counter_defense_01,
  *shadow_attack_defended;

#endif	/* MININIM_GUARD_ATTACK_H */
