/*
  fight.h -- fight module;

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

#ifndef MININIM_FIGHT_H
#define MININIM_FIGHT_H

#include "anim.h"

#define INVERSION_RANGE 18
#define ATTACK_RANGE 61
#define FIGHT_RANGE 75
#define FOLLOW_RANGE (2 * ORIGINAL_WIDTH)
#define HIT_RANGE 48

void fight_ai (struct anim *k0, struct anim *k1);
void fight_mechanics (struct anim *k);
void fight_inversion_mechanics (struct anim *k, struct anim *ke);
bool is_enemy (struct anim *k0, struct anim *k1);
void consider_enemy (struct anim *k0, struct anim *k1);
void forget_enemy (struct anim *k);
struct anim *get_enemy (struct anim *k);
void enter_fight_mode (struct anim *k);
void leave_fight_mode (struct anim *k);
bool is_in_range (struct anim *k0, struct anim *k1, int r);
int dist_enemy (struct anim *k);
void fight_follow (struct anim *k0, struct anim *k1);
bool is_in_fight_mode (struct anim *k);
bool is_attacking (struct anim *k);
bool is_defending (struct anim *k);
bool is_walking (struct anim *k);
bool is_sword_hit (struct anim *k);
bool is_at_defendable_attack_frame (struct anim *k);
bool is_at_hit_frame (struct anim *k);
void put_at_defense_frame (struct anim *k);
void put_at_attack_frame (struct anim *k);
bool is_seeing (struct anim *k0, struct anim *k1);
bool is_hearing (struct anim *k0, struct anim *k1);
bool is_on_back (struct anim *k0, struct anim *k1);
bool is_near (struct anim *k0, struct anim *k1);
bool is_safe_to_walkf (struct anim *k);
bool is_safe_to_walkb (struct anim *k);
bool is_safe_to_attack (struct anim *k);
bool is_safe_to_turn (struct anim *k);
void fight_turn (struct anim *k);
void fight_command_defense (struct anim *k);
void fight_command_attack (struct anim *k);
void fight_command_walkf (struct anim *k);
void fight_command_walkb (struct anim *k);
void fight_walkf (struct anim *k);
void fight_walkb (struct anim *k);
void fight_hit (struct anim *k, struct anim *ke);
void fight_die (struct anim *k);
void fight_fall (struct anim *k);

struct fighter_profile *
get_perfect_fighter_profile (struct fighter_profile *fp);
struct fighter_profile *
get_median_fighter_profile (struct fighter_profile *fp);

#endif	/* MININIM_FIGHT_H */
