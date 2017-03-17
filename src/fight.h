/*
  fight.h -- fight module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

bool are_valid_opponents (struct actor *k0, struct actor *k1);
void leave_fight_logic (struct actor *k);
void enter_fight_logic (struct actor *k);
void fight_ai (struct actor *k);
void fight_mechanics (struct actor *k);
void fight_inversion_mechanics (struct actor *k, struct actor *ke);
void consider_enemy (struct actor *k0, struct actor *k1);
void forget_enemy (struct actor *k);
void enter_fight_mode (struct actor *k);
void leave_fight_mode (struct actor *k);
int dist_actors (struct actor *k0, struct actor *k1);
bool is_in_range (struct actor *k0, struct actor *k1, int r);
int dist_enemy (struct actor *k);
void fight_follow (struct actor *k0, struct actor *k1);
bool is_in_fight_mode (struct actor *k);
bool is_attacking (struct actor *k);
bool is_defending (struct actor *k);
bool is_walking (struct actor *k);
bool is_walkingf (struct actor *k);
bool is_walkingb (struct actor *k);
bool is_sword_hit (struct actor *k);
bool is_at_defendable_attack_frame (struct actor *k);
bool is_at_hit_frame (struct actor *k);
void put_at_defense_frame (struct actor *k);
void put_at_attack_frame (struct actor *k);
bool opaque_cs (enum tile_fg t);
bool is_actor_seeing (struct actor *k0, struct actor *k1, enum dir dir);
bool is_pos_seeing (struct pos *p0, struct actor *k1, enum dir dir);
bool is_hearing (struct actor *k0, struct actor *k1);
bool is_on_back (struct actor *k0, struct actor *k1);
bool is_pos_on_back (struct actor *k, struct pos *p);
bool is_near (struct actor *k0, struct actor *k1);
bool is_safe_to_walkb (struct actor *k);
bool is_safe_to_attack (struct actor *k);
bool is_safe_to_turn (struct actor *k);
bool dangerous_cs (enum tile_fg t);
bool is_safe_to_follow (struct actor *k0, struct actor *k1, enum dir dir);
bool is_there_enough_room_to_fight (struct actor *k);
void fight_turn (struct actor *k);
void fight_turn_controllable (struct actor *k);
void fight_defense (struct actor *k);
void fight_attack (struct actor *k);
void fight_walkf (struct actor *k);
void fight_walkb (struct actor *k);
void fight_hit (struct actor *k, struct actor *ke);
enum tile_fg fight_tile_rel (struct actor *k, coord_f cf, int floor, int place);
void backoff_from_range (struct actor *k0, struct actor *k1, int r,
                         bool only_k1, bool k1_dominant);
void get_in_range (struct actor *k0, struct actor *k1, int r,
                   bool only_k1, bool k1_dominant);
void alert_guards (struct pos *p);

struct skill *get_perfect_skill (struct skill *fp);
struct skill *upgrade_skill (struct skill *s0, struct skill *s1,
                             int total_lives);

#endif	/* MININIM_FIGHT_H */
