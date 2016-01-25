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

bool are_valid_opponents (struct anim *k0, struct anim *k1);
void leave_fight_logic (struct anim *k);
void enter_fight_logic (struct anim *k);
void fight_ai (struct anim *k);
void fight_mechanics (struct anim *k);
void fight_inversion_mechanics (struct anim *k, struct anim *ke);
void consider_enemy (struct anim *k0, struct anim *k1);
void forget_enemy (struct anim *k);
void enter_fight_mode (struct anim *k);
void leave_fight_mode (struct anim *k);
bool is_in_range (struct anim *k0, struct anim *k1, int r);
int dist_enemy (struct anim *k);
void fight_follow (struct anim *k0, struct anim *k1);
bool is_in_fight_mode (struct anim *k);
bool is_attacking (struct anim *k);
bool is_defending (struct anim *k);
bool is_walking (struct anim *k);
bool is_walkingf (struct anim *k);
bool is_sword_hit (struct anim *k);
bool is_at_defendable_attack_frame (struct anim *k);
bool is_at_hit_frame (struct anim *k);
void put_at_defense_frame (struct anim *k);
void put_at_attack_frame (struct anim *k);
bool opaque_cs (enum confg t);
struct pos *get_pos (struct anim *k, struct pos *p, struct coord *m);
bool is_seeing (struct anim *k0, struct anim *k1, enum dir dir);
bool is_hearing (struct anim *k0, struct anim *k1);
bool is_on_back (struct anim *k0, struct anim *k1);
bool is_pos_on_back (struct anim *k, struct pos *p);
bool is_near (struct anim *k0, struct anim *k1);
bool is_safe_to_walkb (struct anim *k);
bool is_safe_to_attack (struct anim *k);
bool is_safe_to_turn (struct anim *k);
bool dangerous_cs (enum confg t);
bool is_safe_to_follow (struct anim *k0, struct anim *k1, enum dir dir);
void fight_turn (struct anim *k);
void fight_turn_controllable (struct anim *k);
void fight_defense (struct anim *k);
void fight_attack (struct anim *k);
void fight_walkf (struct anim *k);
void fight_walkb (struct anim *k);
void fight_hit (struct anim *k, struct anim *ke);
enum confg fight_crel (struct anim *k, int floor, int place);
bool fight_door_split_collision (struct anim *a);
void alert_guards (struct pos *p);

struct skill *get_perfect_skill (struct skill *fp);
struct skill *upgrade_skill (struct skill *s0, struct skill *s1);

#endif	/* MININIM_FIGHT_H */
