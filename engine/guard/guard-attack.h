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
#define GUARD_ATTACK_01 "data/guard/attack/01.png"
#define GUARD_ATTACK_02 "data/guard/attack/02.png"
#define GUARD_ATTACK_03 "data/guard/attack/03.png"
#define GUARD_ATTACK_04 "data/guard/attack/04.png"
#define GUARD_ATTACK_05 "data/guard/attack/05.png"
#define GUARD_ATTACK_06 "data/guard/attack/06.png"
#define GUARD_ATTACK_07 "data/guard/attack/07.png"
#define GUARD_ATTACK_08 "data/guard/attack/08.png"
#define GUARD_ATTACK_COUNTER_DEFENSE_01 "data/guard/attack/counter-defense-01.png"
#define GUARD_ATTACK_COUNTER_DEFENSE_02 "data/guard/attack/counter-defense-02.png"
#define GUARD_ATTACK_DEFENDED "data/guard/attack/defended.png"

/* fat guard */
#define FAT_GUARD_ATTACK_01 "data/fat-guard/attack/01.png"
#define FAT_GUARD_ATTACK_02 "data/fat-guard/attack/02.png"
#define FAT_GUARD_ATTACK_03 "data/fat-guard/attack/03.png"
#define FAT_GUARD_ATTACK_04 "data/fat-guard/attack/04.png"
#define FAT_GUARD_ATTACK_05 "data/fat-guard/attack/05.png"
#define FAT_GUARD_ATTACK_06 "data/fat-guard/attack/06.png"
#define FAT_GUARD_ATTACK_07 "data/fat-guard/attack/07.png"
#define FAT_GUARD_ATTACK_08 "data/fat-guard/attack/08.png"
#define FAT_GUARD_ATTACK_COUNTER_DEFENSE_01 "data/fat-guard/attack/counter-defense-01.png"
#define FAT_GUARD_ATTACK_COUNTER_DEFENSE_02 "data/fat-guard/attack/counter-defense-02.png"
#define FAT_GUARD_ATTACK_DEFENDED "data/fat-guard/attack/defended.png"

#define GUARD_ATTACK_FRAMESET_NMEMB 10
extern struct frameset guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];
extern struct frameset fat_guard_attack_frameset[GUARD_ATTACK_FRAMESET_NMEMB];

void load_guard_attack (void);
void unload_guard_attack (void);
void guard_attack (struct anim *g);
struct frameset *get_guard_attack_frameset (enum anim_type t);
ALLEGRO_BITMAP *get_guard_attack_defended_bitmap (enum anim_type t);

/* guard */
extern ALLEGRO_BITMAP *guard_attack_01, *guard_attack_02,
  *guard_attack_03, *guard_attack_04, *guard_attack_05,
  *guard_attack_06, *guard_attack_07, *guard_attack_08,
  *guard_attack_counter_defense_01, *guard_attack_counter_defense_02,
  *guard_attack_defended;

/* fat guard */
extern ALLEGRO_BITMAP *fat_guard_attack_01, *fat_guard_attack_02,
  *fat_guard_attack_03, *fat_guard_attack_04, *fat_guard_attack_05,
  *fat_guard_attack_06, *fat_guard_attack_07, *fat_guard_attack_08,
  *fat_guard_attack_counter_defense_01, *fat_guard_attack_counter_defense_02,
  *fat_guard_attack_defended;

#endif	/* MININIM_GUARD_ATTACK_H */
