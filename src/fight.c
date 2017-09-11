/*
  fight.c -- fight module;

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

#include "mininim.h"

bool
are_valid_opponents (struct actor *k0, struct actor *k1)
{
  /* no one fights oneself */
  if (k0->id == k1->id) return false;

  /* no one fights one's own fellow shadows */
  if ((k0->id == 0 || k0->shadow_of == 0)
      && (k1->id == 0 || k1->shadow_of == 0))
    return false;

  /* non-fightable characters can't fight */
  if (! is_fightable_actor (k0) || ! is_fightable_actor (k1))
    return false;

  /* non-fighters don't fight */
  if (! k0->fight || ! k1->fight)
    return false;

  /* fight against controllables is fair game */
  if (k0->controllable || k1->controllable) return true;

  return false;
}

void
leave_fight_logic (struct actor *k)
{
  /* dead character doesn't fight */
  if (k->current_hp <= 0) {
    k->enemy_refraction = 0;
    forget_enemy (k);
    return;
  }

  /* non-fightable characters don't fight */
  if (! is_fightable_actor (k)) return;

  /* no enemy, no need to forget */
  if (k->enemy_id == -1) return;

  /* non-fighter doesn't fight */
  if (! k->fight) return;

  /* character that went upstairs doesn't fight */
  if (k->action == kid_stairs) {
    k->enemy_refraction = 0;
    forget_enemy (k);
    return;
  }

  /* who's the enemy? */
  struct actor *ke = get_actor_by_id (k->enemy_id);

  /* if the enemy doesn't exist, forget about it */
  if (! ke) {
    k->enemy_refraction = 0;
    forget_enemy (k);
    return;
  }

  /* if the enemy is dead no need to worry about him */
  if (ke->current_hp <= 0) {
    k->enemy_refraction = 0;
    forget_enemy (k);
    return;
  }

  /* if the enemy went up stairs, forget about him */
  if (ke->action == kid_stairs) {
    k->enemy_refraction = 0;
    forget_enemy (k);
    return;
  }

  /* if the enemy is not reachable, forget about him */
  enum dir odir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  struct pos p, pe;
  survey (_m, pos, &k->f, NULL, &p, NULL);
  survey (_m, pos, &ke->f, NULL, &pe, NULL);
  pos2room (&pe, p.room, &pe);
  if (! is_actor_seeing (k, ke, k->f.dir)
      && ! is_near (k, ke)
      && ! is_safe_to_follow (k, ke, k->f.dir)
      && ! (is_on_back (k, ke) && is_actor_seeing (k, ke, odir)
            && p.floor == pe.floor)) {
    forget_enemy (k);
    return;
  }
}

void
enter_fight_logic (struct actor *k)
{
  /* non-fightable characters don't fight */
  if (! is_fightable_actor (k)) return;

  /* /\* has an enemy, no need to get another *\/ */
  /* if (k->enemy_id != -1 && k->enemy_refraction < 0) */
  /*   return; */

  /* dead character doesn't fight */
  if (k->current_hp <= 0) return;

  /* non-fighter doesn't fight */
  if (! k->fight) return;

  /* give priority to currenty enemy in case he's attacking */
  struct actor *ke = get_reciprocal_enemy (k);
  if (ke && is_in_range (k, ke, FIGHT_RANGE) && is_attacking (ke))
    return;

  int i;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];

    /* no dead character is a valid opponent */
    if (a->current_hp <= 0) continue;

    /* only valid opponents matter */
    if (! are_valid_opponents (k, a)) continue;

    if (k->enemy_id != -1 && k->enemy_refraction < 0) {
      /**** has an enemy already ****/

      /* don't consider current enemy */
      if (k->enemy_id == a->id) continue;

      /* if another enemy is attacking, give priority to him */
      struct actor *ae = get_actor_by_id (a->enemy_id);
      if (is_in_range (k, a, FIGHT_RANGE)
          && is_attacking (a)
          && (a->enemy_id == k->id
              || ! is_in_range (a, ae, FIGHT_RANGE))) {
        if (k->f.dir == a->f.dir) fight_turn (k);
        consider_enemy (k, a);
        return;
      }

      /* /\* below this only guards *\/ */
      /* if (k->id != 0 && k->shadow_of != 0) continue; */

      /* give priority to nearest enemy */
      struct actor *ke = get_actor_by_id (k->enemy_id);
      int da = dist_actors (k, a);
      int de = dist_actors (k, ke);

      if (da < de
          && abs (da - de) >= PLACE_WIDTH
          && ! is_on_back (k, a)
          && (((de > FIGHT_RANGE + PLACE_WIDTH || is_in_fight_mode (a))
               && a->enemy_id == k->id) || ke->enemy_id != k->id)) {
        consider_enemy (k, a);
        return;
      }

      /* give priority to unpaired enemy */
      struct actor *kee = get_reciprocal_enemy (ke);
      if (kee && kee->id != k->id
          && (is_safe_to_follow (kee, ke, kee->f.dir)
              || ! is_safe_to_follow (k, ke, k->f.dir))
          && (is_actor_seeing (k, a, k->f.dir)
              || is_hearing (k, a))) {
        consider_enemy (k, a);
        return;
      }
    } else {
      /**** has no enemy ****/

      /* if seeing the character consider him an enemy */
      if (is_actor_seeing (k, a, k->f.dir)) {
        consider_enemy (k, a);
        return;
      }

      /* if feeling the character's presence consider him an enemy */
      if (is_near (k, a)) {
        consider_enemy (k, a);
        return;
      }

      /* if forgetting about an enemy look for a reachable opponent */
      if (is_in_fight_mode (k)
          && (k->id != 0 && k->shadow_of != 0)
          && (a->id == 0 || a->shadow_of == 0)
          && is_actor_seeing (k, a, opposite_dir (k->f.dir))) {
        fight_turn (k);
        return;
      }
    }
  }
}

void
fight_ai (struct actor *k)
{
  /* dead characters don't fight */
  if (k->current_hp <= 0) return;

  /* non-fightable characters don't fight */
  if (! is_fightable_actor (k)) return;

  /* controllables and non-fighters doesn't need AI to fight */
  if (k->controllable || ! k->fight) return;

  /* for all characters... */
  int i;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];

    /* no dead character is a valid opponent */
    if (a->current_hp <= 0) continue;

    /* only valid opponents matter */
    if (! are_valid_opponents (k, a)) continue;

    /* without an enemy keep attention to noises */
    struct actor *ke = get_actor_by_id (k->enemy_id);
    int da = dist_actors (k, a);
    int de = dist_actors (k, ke);

    if ((! ke
         || ((da < de && abs (da - de) >= 8)
             && (de > FIGHT_RANGE + PLACE_WIDTH
                 || ! is_safe_to_follow (k, ke, k->f.dir))))
        && is_hearing (k, a)
        && is_on_back (k, a)
        && anim_cycle - k->alert_cycle > 24) {
      if (is_in_fight_mode (k)) fight_turn (k);
      else k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      k->alert_cycle = anim_cycle;
      return;
    }
  }

  /* if forgetting about an enemy do nothing */
  if (k->enemy_refraction > 0) return;

  /* without an enemy or awareness, no need to fight */
  if (k->enemy_id == -1) {
    if (is_in_fight_mode (k)) leave_fight_mode (k);
    return;
  }

  /* first thing, enter in fight mode */
  if (! is_in_fight_mode (k)) {
    enter_fight_mode (k);
    return;
  }

  /* who's the enemy? */
  struct actor *ke = get_actor_by_id (k->enemy_id);

  /* what's the facing opposite direction? */
  enum dir odir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  /* get positions */
  struct pos p, pe;
  survey (_m, pos, &k->f, NULL, &p, NULL);
  survey ((k->f.dir != ke->f.dir) ? _m : _mba,
          pos, &ke->f, NULL, &pe, NULL);
  pos2room (&pe, p.room, &pe);

  /* remember the place the enemy was last seen */
  if (pe.room == p.room && pe.floor == p.floor)
    k->enemy_pos = pe;
  else if (pe.floor > p.floor && is_valid_pos (&k->enemy_pos)
           && ! is_traversable (&k->enemy_pos))
    k->enemy_pos.place = pe.place;

  if (is_valid_pos (&k->enemy_pos)
      && is_traversable (&k->enemy_pos)) {
    struct pos pp;
    int d = (p.place < k->enemy_pos.place) ? -1 : +1;
    while (is_traversable (prel (&k->enemy_pos, &pp, +0, d)))
      k->enemy_pos = pp;
  }

  /* prevent enemy from passing through */
  if (ke->type == KID
      && is_in_range (k, ke, ATTACK_RANGE)
      && ! is_in_fight_mode (ke)
      && ! ke->immortal
      && ! (is_kid_climb (ke) && ke->i <= 7)
      && ke->f.dir != k->f.dir
      && ke->current_hp > 0
      && ke->action != kid_fall
      && ! is_kid_hang (ke)
      && ! ((ke->action == kid_jump || ke->action == kid_run_jump)
            && traversable_cs
            (fight_tile_rel (ke, _m, +0, +0)))
      && is_safe_to_attack (ke)) {
    if (ke->has_sword) {
      place_on_the_ground (&ke->f, &ke->f.c);
      if (k->f.dir != ke->f.dir) {
        if (is_falling (&ke->f, _tb, +0, +0))
          backoff_from_range (ke, k, FIGHT_RANGE, true, false);
        else
          backoff_from_range (k, ke, FIGHT_RANGE, true, false);
      }
      kid_take_sword (ke);
      ke->auto_taken_sword = true;
      ke->no_walkf_timer = 6;
    } else if (is_attacking (k) && ! ke->sword_immune) {
      k->i = 3;
      place_on_the_ground (&ke->f, &ke->f.c);
      fight_hit (ke, k);
    }
  }

  /* prevent enemy from hiding near */
  if (ke->type == KID
      && is_in_range (k, ke, INVERSION_RANGE)
      && ! is_in_fight_mode (ke)
      && ke->current_hp > 0) {
    if (is_safe_to_walkb (k)) fight_walkb (k);
    else if (is_safe_to_turn (k)) fight_turn (k);
    return;
  }

  /* if the enemy is on the back, turn */
  if (is_on_back (k, ke) && is_actor_seeing (k, ke, odir)
      && p.floor == pe.floor
      && ! is_in_range (k, ke, INVERSION_RANGE)) {
    if (is_safe_to_turn (k)) fight_turn (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* if too near to a wall, back off to have room for an attack */
  if (fight_tile_rel (k, _m, +0, +1) == WALL
      && is_safe_to_walkb (k)) {
    fight_walkb (k);
    return;
  }

  /* /\* if the enemy can be followed in the opposite direction, turn *\/ */
  /* if (! is_safe_to_follow (k, ke, k->f.dir) */
  /*     && is_safe_to_follow (k, ke, odir)) { */
  /*   fight_turn (k); */
  /*   return; */
  /* } */

  /* if the enemy is trying to bypass, attack him */
  if (! is_in_fight_mode (ke)
      && ! ke->has_sword
      && ke->action != kid_stairs
      && ke->f.dir != k->f.dir
      && ke->current_hp > 0
      && ! is_on_back (k, ke)
      && ((ke->action == kid_run
           && is_in_range (k, ke, 3 * PLACE_WIDTH - 4))
          || (ke->action == kid_run_jump
              && is_in_range (k, ke, 4 * PLACE_WIDTH - 4))
          || (is_kid_jump_air (ke) && ke->i < 9
              && is_in_range (k, ke, 4 * PLACE_WIDTH)))) {
    if (is_safe_to_attack (k)) fight_attack (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* stays at least in the fight range.  Advance, unless the enemy is
     not running towards you */
  if (! is_in_range (k, ke, FIGHT_RANGE + 10)
      && ke->action != kid_stairs
      && (ke->f.dir == k->f.dir
          || p.room != pe.room
          || p.floor != pe.floor
          || ! (ke->action == kid_run
                || ke->action == kid_run_jump
                || ke->action == kid_jump))
      && is_safe_to_follow (k, ke, k->f.dir)) {
    fight_walkf (k);
    return;
  }

  /* if the enemy is not targeting you, do nothing more */
  struct actor *kee = get_reciprocal_enemy (ke);
  if (kee && kee->id != k->id) return;

  /* in fight range, if the enemy is not attacking, go towards attack
     range (with probability, unless the enemy is not in fight mode,
     then go immediately) */
  if (is_in_range (k, ke, FIGHT_RANGE + 10)
      && ! is_in_range (k, ke, ATTACK_RANGE)
      && ke->action != kid_stairs
      && is_safe_to_follow (k, ke, k->f.dir)
      && (prandom (99) <= k->skill.advance_prob
          || prandom (99) < k->angry
          || ! is_in_fight_mode (ke))
      && ! is_attacking (ke)
      && (! is_in_fight_mode (ke)
          || is_walking (ke)
          || ke->i >= 6 + prandom (24))) {
    fight_walkf (k);
    return;
  }

  /* in attack range, if being attacked, defend yourself (with
     probability) and counter attack (with probability handled
     elsewhere) */
  if (is_in_range (k, ke, ATTACK_RANGE + 16)
      && ! is_on_back (k, ke)
      && (is_attacking (ke) && ke->i == 0)
      && (prandom (99) <= k->skill.defense_prob
          || prandom (99) < k->angry
          || k->refraction > 0)
      && ke->current_hp > 0) {
    fight_defense (k);
    fight_attack (k);
    return;
  }

  /* if attacking, counter defend
     (with probability handled elsewhere) */
  if (is_in_range (k, ke, ATTACK_RANGE + 16)
      && is_attacking (k)) {
    fight_defense (k);
    fight_attack (k);
    return;
  }

  /* in attack range, if not being attacked, attack (with probability,
     unless the enemy is not in fight mode, then attack immediately) */
  if (is_in_range (k, ke, ATTACK_RANGE)
      && ! is_attacking (ke)
      && ! is_on_back (k, ke)
      && ke->action != kid_stairs
      && ! ((is_kid_climb (ke)
             || is_kid_successfully_climbing (ke))
            && ke->i >= 1)
      && ke->current_hp > 0
      && (prandom (99) <= k->skill.attack_prob
          || ! is_in_fight_mode (ke))) {
    if (is_safe_to_attack (k)) fight_attack (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* in hit range, back off (with probability) */
  if (is_in_range (k, ke, HIT_RANGE - 10)
      && is_safe_to_walkb (k)
      && ! k->refraction
      && ! is_walkingb (ke)
      && prandom (99) <= k->skill.return_prob) {
    fight_walkb (k);
    return;
  }
}

void
fight_mechanics (struct actor *k)
{
  if (k->sword_immune > 0) k->sword_immune--;

  /* 'ke' is the attacking part */
  struct actor *ke = get_actor_by_id (k->enemy_id);

  if (! ke || ke->enemy_id != k->id) return;

  fight_inversion_mechanics (k, ke);

  if (! is_attacking (ke) || is_sword_hit (k)) return;

  if (! ke->attack_range_far)
    ke->attack_range_far =
      (is_in_range (k, ke, ATTACK_RANGE + 16) && ke->i == 0);

  if (! ke->attack_range_near)
    ke->attack_range_near =
      (is_in_range (k, ke, ATTACK_RANGE) && ke->i == 0);

  if (! ke->enemy_defended_my_attack)
    ke->enemy_defended_my_attack =
      (ke->i < 4 && k->key.up && ke->attack_range_far)
      || (ke->attack_range_far && ! ke->attack_range_near);

  if (is_walkingf (k) || is_attacking (k) || ! is_in_fight_mode (k))
    ke->enemy_defended_my_attack = 0;

  bool walkb = ((k->f.dir == RIGHT) && k->key.left)
    || ((k->f.dir == LEFT) && k->key.right);

  if (! ke->enemy_counter_attacked_myself)
    ke->enemy_counter_attacked_myself =
      (ke->enemy_defended_my_attack
       && (k->key.shift || ! k->controllable)
       && (prandom (99) <= k->skill.counter_attack_prob
           || prandom (99) < k->angry)
       && (ke->attack_range_near || k->attack_range_near)
       && ! walkb);

  if (! ke->i_counter_defended)
    ke->i_counter_defended =
      (ke->enemy_counter_attacked_myself && ke->key.up
       && (prandom (99) <= ke->skill.counter_defense_prob
           || k->angry));

  /* /\* DEBUG: endless counter attack/defense chain ***** *\/ */
  /* ke->enemy_counter_attacked_myself = true; */
  /* ke->i_counter_defended = true; */
  /* /\* **************************** *\/ */

  /* /\* DEBUG: no defense whatsoever *\/ */
  /* ke->enemy_defended_my_attack = 0; */
  /* /\* **************************** *\/ */

  if (! k->hurt_enemy_in_counter_attack)
    k->hurt_enemy_in_counter_attack =
      (ke->enemy_counter_attacked_myself && ! ke->i_counter_defended);

  /* printf ("ke->attack_range_near: %i\n", ke->attack_range_near); */

  /* printf ("ke->enemy_defended_my_attack = %i, ke->i = %i, k->key.up = %i\n", */
  /*         ke->enemy_defended_my_attack, ke->i, k->key.up); */

  ke->placed_at_attack_frame = false;

  if (! ke->enemy_defended_my_attack
      && is_at_hit_frame (ke)
      && ! is_on_back (ke, k)
      && (ke->attack_range_far || ! is_in_fight_mode (k))
      && ((is_in_range (k, ke, HIT_RANGE + 4) && is_in_fight_mode (k))
          || (is_in_range (k, ke, ATTACK_RANGE) && ! is_in_fight_mode (k))))
    fight_hit (k, ke);
  else if (ke->hurt_enemy_in_counter_attack
           && is_at_hit_frame (ke)
           && ! is_on_back (ke, k))
    fight_hit (k, ke);
  else if (! ke->hurt_enemy_in_counter_attack
           && ke->enemy_defended_my_attack == 1
           && is_at_defendable_attack_frame (ke)) {
    if (is_in_range (k, ke, HIT_RANGE + 8)
        || is_defending (k)) {
      backoff_from_range (ke, k, ATTACK_RANGE - 20, false, false);
      get_in_range (ke, k, ATTACK_RANGE - 6, false, false);
      put_at_attack_frame (ke);
      put_at_defense_frame (k);
      ke->placed_at_attack_frame = true;
    } else ke->enemy_defended_my_attack = false;
  }

/*   printf ("id: %i, ad: %i, ca: %i, cd: %i, i: %i, hurt: %i\n\ */
/* id: %i, ad: %i, ca: %i, cd: %i, i: %i, hurt: %i\n\ */
/* -------------------------------\n", */
/*           k->id, k->enemy_defended_my_attack, k->enemy_counter_attacked_myself, k->i_counter_defended, k->i, k->hurt, */
/*           ke->id, ke->enemy_defended_my_attack, ke->enemy_counter_attacked_myself, ke->i_counter_defended, ke->i, ke->hurt); */
}

void
fight_inversion_mechanics (struct actor *k, struct actor *ke)
{
  if (is_in_range (k, ke, INVERSION_RANGE)
      && is_in_fight_mode (k)
      && is_in_fight_mode (ke)
      && ! is_sword_hit (k)
      && ! is_sword_hit (ke)
      && (! is_attacking (k) || k->i > 4)
      && (! is_attacking (ke) || k->i > 4)) {
    struct coord c;
    c = k->f.c;
    k->f.c = ke->f.c;
    ke->f.c = c;

    if (k->f.dir == ke->f.dir) {
      if (is_on_back (k, ke))
        invert_frame_dir (&k->f, &k->f);
      else invert_frame_dir (&ke->f, &ke->f);
    }

    place_on_the_ground (&k->f, &k->f.c);
    place_on_the_ground (&ke->f, &ke->f.c);

    fight_turn (k);
    fight_turn (ke);

    backoff_from_range (ke, k, INVERSION_RANGE + 4, false, false);
  }
}

void
consider_enemy (struct actor *k0, struct actor *k1)
{
  k0->enemy_id = k1->id;
  k0->enemy_refraction = -1;
}

void
forget_enemy (struct actor *k)
{
  if (k->enemy_refraction > 0) return;
  else if (k->enemy_refraction < 0) {
    k->enemy_refraction = ENEMY_REFRACTION_TIME;
    return;
  }

  k->enemy_id = -1;
}

void
enter_fight_mode (struct actor *k)
{
  if (! k->has_sword) return;
  k->key.enter = true;
}

void
leave_fight_mode (struct actor *k)
{
  k->key.down = true;
}

lua_Number
dist_actors (struct actor *k0, struct actor *k1)
{
  if (! k0 || ! k1) return INT_MAX;

  struct coord m0, m1; struct pos p0, p1;
  survey (_m, pos, &k0->f, &m0, &p0, NULL);
  survey (_m, pos, &k1->f, &m1, &p1, NULL);
  coord2room (&m1, m0.room, &m1);
  pos2room (&p1, p0.room, &p1);

  if (m1.room == m0.room
      && p1.floor == p0.floor)
    return fabs (m1.x - m0.x);
  else return INT_MAX;
}

int
dist_enemy (struct actor *k)
{
  struct actor *ke = get_actor_by_id (k->enemy_id);
  if (! ke) return INT_MAX;
  return dist_actors (k, ke);
}

bool
is_in_range (struct actor *k0, struct actor *k1, int r)
{
  int d = dist_actors (k0, k1);
  return d < r;
}

enum tile_fg
fight_tile_rel (struct actor *k, coord_f cf, int floor, int place)
{
  struct pos p;
  survey (cf, pos, &k->f, NULL, &p, NULL);

  /* place sign indicates direction in relation to k orientation */
  int dir = (k->f.dir == LEFT) ? -1 : +1;
  return fg_rel (&p, floor, dir * place);
}

bool
is_in_fight_mode (struct actor *k)
{
  return k->action == kid_sword_normal
    || k->action == kid_sword_walkf
    || k->action == kid_sword_walkb
    || k->action == kid_sword_attack
    || k->action == kid_sword_defense
    || k->action == kid_take_sword
    || k->action == kid_sword_hit

    || k->action == guard_sword_normal
    || k->action == guard_sword_walkf
    || k->action == guard_sword_walkb
    || k->action == guard_sword_attack
    || k->action == guard_sword_defense
    || k->action == guard_sword_hit;
}

bool
is_attacking (struct actor *k)
{
  if (! k) return false;
  else return (k->action == kid_sword_attack && k->i <= 4)
         || (k->action == guard_sword_attack && k->i <= 4)
         || ((k->action == kid_sword_normal
              || k->action == guard_sword_normal)
             && k->key.shift && ! k->key.up);
}

bool
is_defending (struct actor *k)
{
  if (! k) return false;
  else return k->action == kid_sword_defense
         || k->action == guard_sword_defense;
}

bool
is_walking (struct actor *k)
{
  if (! k) return false;
  else return k->action == kid_sword_walkf
         || k->action == kid_sword_walkb
         || k->action == guard_sword_walkf
         || k->action == guard_sword_walkb;
}

bool
is_walkingb (struct actor *k)
{
  if (! k) return false;
  else return k->action == kid_sword_walkb
         || k->action == guard_sword_walkb;
}

bool
is_walkingf (struct actor *k)
{
  if (! k) return false;
  else return k->action == kid_sword_walkf
         || k->action == guard_sword_walkf;
}

bool
is_sword_hit (struct actor *k)
{
  if (! k) return false;
  else return k->action == kid_sword_hit
         || k->action == guard_sword_hit;
}

bool
is_at_defendable_attack_frame (struct actor *k)
{
  return (k->action == kid_sword_attack && k->i == 2)
    || (k->action == guard_sword_attack && k->i == 3);
}

bool
is_at_hit_frame (struct actor *k)
{
  return (k->action == kid_sword_attack && k->i == 3)
    || (k->action == guard_sword_attack && k->i == 4);
}

void
put_at_defense_frame (struct actor *k)
{
  struct actor *ke = get_actor_by_id (k->enemy_id);

  play_audio (&sword_defense_audio, NULL, k->id);
  stop_audio_instance (&sword_attack_audio, NULL, k->enemy_id);
  kid_haptic (k, KID_HAPTIC_SWORD);
  kid_haptic (ke, KID_HAPTIC_SWORD);

  switch (k->type) {
  case NO_ACTOR: default: break;
  case KID:
    select_actor_frame (k, "KID", "SWORD_DEFENSE", 0);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    select_actor_frame (k, "KID", "SWORD_DEFENSE", 1);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    if (ke->type == KID)
      select_actor_xframe (k, "KID", "SWORD", 11);
    else select_actor_xframe (k, "KID", "SWORD", 14);

    k->action = kid_sword_defense;
    uncollide_back_fight (k);
    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    select_actor_frame (k, NULL, "SWORD_DEFENSE", 0);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    select_actor_xframe (k, NULL, "SWORD", 11);
    k->action = guard_sword_defense;
    uncollide_back_fight (k);
    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    break;
  }

  /* if (k->id == 0) */
  /*   printf ("%s: k->i = %i, k->fo.dx = %i\n", */
  /*           __func__, k->i, k->fo.dx); */
}

void
put_at_attack_frame (struct actor *k)
{
  k->enemy_defended_my_attack = 2;
  k->enemy_counter_attacked_myself =
    k->enemy_counter_attacked_myself ? 2 : 0;
  k->i_counter_defended = k->i_counter_defended ? 2 : 0;

  switch (k->type) {
  case NO_ACTOR: default: break;
  case KID:
    if (k->i == 3) return;

    k->f = k->of;

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    select_actor_frame (k, "KID", "SWORD_ATTACK", 0);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    select_actor_frame (k, "KID", "SWORD_ATTACK", 1);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    select_actor_frame (k, "KID", "SWORD_ATTACK", 2);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    /* sword attack defended */
    k->i = 8;

    k->fo.b = actor_bitmap (k, "KID", "SWORD_ATTACK", 8);
    k->fo.dx += actor_dx (k, "KID", "SWORD_ATTACK", 8);
    k->fo.dy += actor_dy (k, "KID", "SWORD_ATTACK", 8);

    select_actor_xframe (k, "KID", "SWORD", 17);

    uncollide_back_fight (k);
    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    k->i = 3;
    break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    k->fo.b = actor_bitmap (k, NULL, "SWORD_ATTACK", 10);
    k->fo.dx = actor_dx (k, NULL, "SWORD_ATTACK", 10);
    k->fo.dy = actor_dy (k, NULL, "SWORD_ATTACK", 10);

    select_actor_xframe (k, NULL, "SWORD", 8);
    uncollide_back_fight (k);
    next_frame (&k->f, &k->f, &k->fo);

    /* place on the ground */
    place_on_the_ground (&k->f, &k->f.c);

    k->i = 4;
    break;
  }
}

bool
opaque_cs (enum tile_fg t)
{
  return t == WALL || t == CARPET || t == TCARPET || t == MIRROR;
}

bool
is_opaque_at_left (struct pos *p)
{
  enum tile_fg t = fg (p);
  return t == WALL || t == MIRROR;
}

bool
is_opaque_at_right (struct pos *p)
{
  enum tile_fg t = fg (p);
  return t == WALL || t == CARPET || t == TCARPET;
}

bool
is_actor_seeing (struct actor *k0, struct actor *k1, enum dir dir)
{
  struct pos p0; survey (_m, pos, &k0->f, NULL, &p0, NULL);
  struct pos p1; survey (_m, pos, &k1->f, NULL, &p1, NULL);
  if (semantics == LEGACY_SEMANTICS) {
    if (k0->enemy_id == k1->id)
      return is_pos_seeing (&p0, k1, dir);
    else return is_pos_seeing (&p0, k1, dir)
           && p0.room == p1.room;
  } else return is_pos_seeing (&p0, k1, dir);
}

bool
is_pos_seeing (struct pos *p0, struct actor *k1, enum dir dir)
{
  struct coord m0, m1, mt1, mb1; struct pos p, p1, pk, pke;
  tile_coord (p0, _m, &m0);

  coord_f cf;
  if (is_kid_climb (k1) || is_actor_fall (k1)) {
    coord2room (_mt (&k1->f, &mt1), m0.room, &mt1);
    coord2room (_m (&k1->f, &m1), m0.room, &m1);
    coord2room (_mbo (&k1->f, &mb1), m0.room, &mb1);

    lua_Number dt, dm, db;
    dt = (mt1.room == m0.room) ? dist_coord (&m0, &mt1) : INFINITY;
    dm = (m1.room == m0.room) ? dist_coord (&m0, &m1) : INFINITY;
    db = (mb1.room == m0.room) ? dist_coord (&m0, &mb1) : INFINITY;

    if (dt <= dm && dt <= db)
      cf = (dir == LEFT) ? _tr : _tl;
    else if (db <= dt && db <= dm)
      cf = (dir == LEFT) ? _br : _bl;
    else cf = (dir == LEFT) ? _mr : _ml;
  } else cf = (dir == LEFT) ? _mr : _ml;

  surveyo (cf, -8, +0, pos, &k1->f, &m1, NULL, NULL);

  coord2room (&m0, p0->room, &m0);
  coord2room (&m1, p0->room, &m1);
  pos (&m1, &p1);

  if (dir == LEFT) {
    if (is_opaque_at_left (p0)) return false;
    if (is_opaque_at_right (&p1)) return false;
    if (peqr (&p1, p0, +0, -1)) return true;
    prel (p0, &pk, +0, -1);
    prel (&p1, &pke, +0, +1);
  } else {
    if (is_opaque_at_right (p0)) return false;
    if (is_opaque_at_left (&p1)) return false;
    if (peqr (&p1, p0, +0, +1)) return true;
    prel (p0, &pk, +0, +1);
    prel (&p1, &pke, +0, -1);
  }

  if (peq (p0, &p1)) return true;

  first_tile_fg (&pk, &pke, opaque_cs, &p);

  return p0->room == p1.room
    && m1.room == m0.room
    && p1.floor == p0->floor
    && ! (dir == LEFT && m1.x > m0.x)
    && ! (dir == RIGHT && m1.x < m0.x)
    && p.room == -1;
}

bool
is_hearing (struct actor *k0, struct actor *k1)
{
  struct pos p0, p1;
  survey (_m, pos, &k0->f, NULL, &p0, NULL);
  survey ((k0->f.dir == LEFT) ? _mr : _ml,
          pos, &k1->f, NULL, &p1, NULL);

  pos2room (&p1, p0.room, &p1);

  return p1.room == p0.room
    && (k1->action == kid_run
        || k1->action == kid_stop_run
        || is_kid_jump_landing (k1)
        || is_kid_run_jump_running (k1)
        || is_kid_run_jump_landing (k1)
        || (k1->action == kid_crouch && k1->fall)
        || k1->action == kid_take_sword);
}

bool
is_pos_on_back (struct actor *k, struct pos *p)
{
  struct pos pm, pv;
  survey (_m, pos, &k->f, NULL, &pm, NULL);

  pos2room (p, pm.room, &pv);

  return pm.room == pv.room
    && ((k->f.dir == LEFT && pv.place > pm.place)
        || (k->f.dir == RIGHT && pv.place < pm.place));
}

bool
is_on_back (struct actor *k0, struct actor *k1)
{
  struct coord m0, m1;

  coord_f cf = k0->f.dir == LEFT ? _mr : _ml;

  _mf (&k0->f, &m0);
  cf (&k1->f, &m1);
  coord2room (&m1, m0.room, &m1);

  return m1.room == m0.room
    && ((k0->f.dir == LEFT && m1.x > m0.x + IS_ON_BACK_THRESHOLD)
        || (k0->f.dir == RIGHT && m1.x < m0.x - IS_ON_BACK_THRESHOLD));
}

bool
is_near (struct actor *k0, struct actor *k1)
{
  struct coord m0, m1; struct pos p0, p1;
  survey (_m, pos, &k0->f, &m0, &p0, NULL);
  survey (_m, pos, &k1->f, &m1, &p1, NULL);

  _m (&k0->f, &m0);
  _m (&k1->f, &m1);
  coord2room (&m1, m0.room, &m1);

  return m1.room == m0.room && p1.floor == p0.floor
    && fabs (m1.x - m0.x) < PLACE_WIDTH
    && fabs (m1.y - m0.y) < PLACE_HEIGHT;
}

bool
is_safe_to_walkb (struct actor *k)
{
  int df = dist_fall (&k->f, true);
  int dc = dist_collision (&k->f, _bb, +0, +0, &k->ci);
  return df > PLACE_WIDTH && dc > 4;
}

bool
is_safe_to_attack (struct actor *k)
{
  int df = dist_fall (&k->f, false);
  return df > PLACE_WIDTH;
}

bool
is_safe_to_turn (struct actor *k)
{
  int df = dist_fall (&k->f, false);
  return (df > PLACE_WIDTH);
}

bool
dangerous_cs (enum tile_fg t)
{
  return critical_cs (t)
    || t == WALL || t == CARPET || t == TCARPET || t == MIRROR || t == CHOMPER;
}

bool
door_cs (enum tile_fg t)
{
  return t == DOOR;
}

bool
is_safe_at_right (struct pos *p, struct frame *f)
{
  return ! is_collidable_at_right (p, f)
    || (fg (p) == DOOR && door_at_pos (p)->action == OPEN_DOOR);
}

bool
is_safe_at_left (struct pos *p)
{
  enum tile_fg t = fg (p);

  return t != WALL && t != CHOMPER && t != MIRROR;
}

bool
is_safe_to_follow (struct actor *k0, struct actor *k1, enum dir dir)
{
  /* not aware of enemy position */
  if (! is_valid_pos (&k0->enemy_pos)) return false;

  struct pos p0, p, p1, pke, pk;
  survey (_m, pos, &k0->f, NULL, &p0, NULL);
  survey ((k0->f.dir == LEFT) ? _mr : _ml,
          pos, &k1->f, NULL, &p1, NULL);

  pos2room (&p1, p0.room, &p1);

  /* not reachable, to start with */
  if (p0.room != p1.room) return false;
  if (p0.floor > p1.floor) return false;

  /* the character went down after the enemy */
  if (p0.floor != k0->enemy_pos.floor) {
    /* enum dir odir = (dir == LEFT) ? RIGHT : LEFT; */
    /* if (is_on_back (k0, k1) && is_seeing (k0, k1, odir) */
    /*     && p0.floor == p1.floor) return true; */
    if (is_actor_seeing (k0, k1, LEFT)
        || is_actor_seeing (k0, k1, RIGHT)) return true;
    return false;
  }

  /* if falling the chasing is inevitable (necessary to prevent
     leave_fight_logic from forgeting enemy based on the facing
     direction of the is_in_range check) */
  if (is_actor_fall (k0)) return true;

  if (dir == LEFT) {
    if (! is_safe_at_left (&p0)) return false;
    if (! is_safe_at_right (&k0->enemy_pos, &k0->f)) return false;
    if (peqr (&k0->enemy_pos, &p0, +0, -1)) return true;
    prel (&p0, &pk, +0, -1);
    prel (&k0->enemy_pos, &pke, +0, +1);
    if (is_collidable_at_right (&pk, &k0->f))
      return false;
  } else {
    if (! is_safe_at_right (&p0, &k0->f)) return false;
    if (! is_safe_at_left (&k0->enemy_pos)) return false;
    if (peqr (&k0->enemy_pos, &p0, +0, +1)) return true;
    prel (&p0, &pk, +0, +1);
    prel (&k0->enemy_pos, &pke, +0, -1);
    if (is_collidable_at_right (&p0, &k0->f))
      return false;
  }

  /* enemy went down */
  if (is_critical (&k0->enemy_pos)) {
    prel (&k0->enemy_pos, &pke, +1, +0);
    if (is_critical (&pke) || fg (&pke) == SPIKES_FLOOR)
      return false;
    int d = (dir == LEFT) ? -1 : +1;
    if (peq (&pk, &k0->enemy_pos)) return true;
    prel (&k0->enemy_pos, &pke, +0, -1 * d);
  } else if (peq (&p0, &k0->enemy_pos)
             && ! (is_actor_seeing (k0, k1, LEFT)
                   || is_actor_seeing (k0, k1, RIGHT)))
    return false;

  first_tile_fg (&pk, &pke, dangerous_cs, &p);
  if (is_valid_pos (&p)) return false;

  first_tile_fg (&pk, &pke, door_cs, &p);
  if (! is_valid_pos (&p)) return true;
  else return door_at_pos (&p)->action == OPEN_DOOR
         || ! is_collidable_at_right (&p, &k0->f);
}

bool
is_there_enough_room_to_fight (struct actor *k)
{
  return dist_collision (&k->f, _bf, +0, +0, &k->ci) > PLACE_WIDTH - 16
    || dist_collision (&k->f, _bb, +0, +0, &k->ci) > PLACE_WIDTH - 16;
}

void
fight_turn (struct actor *k)
{
  invert_frame_dir (&k->f, &k->f);

  if (! is_in_fight_mode (k))
    enter_fight_mode (k);
  else {
    struct pos p;
    struct actor a = *k;
    a.i = -1;
    actor_walkb (&a);
    survey (_bb, pos, &a.f, NULL, &p, NULL);
    if (! is_traversable (&p)
        && is_there_enough_room_to_fight (&a)) *k = a;
  }
}

void
fight_turn_controllable (struct actor *k)
{
  int d = INT_MAX;
  int t; /* threshold */
  struct actor *ke = NULL;
  struct actor *ke0 = get_actor_by_id (k->enemy_id);

  /* make the kid target the nearest enemy targeting him */
  size_t i;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    if (a->enemy_id != k->id || ! is_fightable_actor (a))
      continue;
    int de = dist_enemy (a);
    if (de < d) {
      d = de;
      ke = a;
    }
  }

  if (ke && ke0 && ke->f.dir != ke0->f.dir) t = 20;
  else t = -1;

  if (ke && abs (dist_enemy (k) - d) > t) {
    for (i = 0; i < actor_nmemb; i++) {
      struct actor *a = &actor[i];
      if (a->id == k->id || (a->id != 0 && a->shadow_of != 0))
        continue;
      int da = dist_actors (k, a);
      if (da < d
          /* && abs (da - d) >= 8 */
          && a->enemy_id == ke->id
          )
        return;
    }
    consider_enemy (k, ke);
  }

  ke = get_actor_by_id (k->enemy_id);
  if (ke) {
    struct pos p, pe;
    survey (_m, pos, &k->f, NULL, &p, NULL);
    survey (_m, pos, &ke->f, NULL, &pe, NULL);
    pos2room (&pe, p.room, &pe);
    if (is_on_back (k, ke)
        && ! is_in_range (k, ke, INVERSION_RANGE)
        && is_in_fight_mode (k)
        && is_in_fight_mode (ke)
        && p.room == pe.room
        && p.floor == pe.floor)
      fight_turn (k);
  }
}

void
fight_defense (struct actor *k)
{
  k->key.up = true;
}

void
fight_attack (struct actor *k)
{
  k->key.shift = true;
}

void
fight_walkf (struct actor *k)
{
  if (k->f.dir == LEFT) k->key.left = true;
  else k->key.right = true;
}

void
fight_walkb (struct actor *k)
{
  if (k->f.dir == LEFT) k->key.right = true;
  else k->key.left = true;
}

void
fight_hit (struct actor *k, struct actor *ke)
{
  if (k->immortal || k->sword_immune) return;
  if (k->current_hp <= 0) return;
  if (is_actor_fall (k) || k->action == kid_stairs)
    return;

  place_on_the_ground (&k->f, &k->f.c);
  k->xf.b = NULL;

  if (! is_in_fight_mode (k)) k->current_hp = 0;
  else k->current_hp--;

  if (! is_guard (ke))
    upgrade_skill (&ke->skill, &k->skill, k->total_hp);

  int d = (k->f.dir == LEFT) ? +1 : -1;
  struct pos pb;
  survey (_m, pos, &k->f, NULL, &k->p, NULL);
  prel (&k->p, &pb, 0, d);

  if (is_attacking (k))
    move_frame (&k->f, _tb, +0, -8, -8);

  if (k->current_hp <= 0 && ! is_traversable (&pb)) {
    k->current_hp = 0;
    k->death_reason = FIGHT_DEATH;
    ke->alert_cycle = anim_cycle;
    /* prevent kid from passing through a collidable */
    if (is_kid_hang (k))
      k->f.c.x += k->f.dir == LEFT ? +8 : -8;
    actor_die (k);
  } else actor_sword_hit (k);

  if (k->f.dir != ke->f.dir) {
    backoff_from_range (ke, k, ATTACK_RANGE - 20, true, false);
    get_in_range (ke, k, ATTACK_RANGE - 10, false, false);
  }

  k->splash = true;

  kid_haptic (k, KID_HAPTIC_HARM);
  kid_haptic (ke, KID_HAPTIC_HARM);

  if (k->id == current_kid_id) {
    global_mr.flicker = 2;
    global_mr.color = get_flicker_blood_color ();
    play_audio (&harm_audio, NULL, k->id);
  } else play_audio (&guard_hit_audio, NULL, k->id);

  stop_audio_instance (&sword_attack_audio, NULL, ke->id);
}

void
backoff_from_range (struct actor *k0, struct actor *k1, int r,
                    bool only_k1, bool k1_dominant)
{
  struct actor *kd, *ks, *kl, *kr;

  kd = k1_dominant ? k1 : k0;
  ks = k1_dominant ? k0 : k1;

  kl = (kd->f.dir == LEFT) ? kd : ks;
  kr = (kd->f.dir == RIGHT) ? kd : ks;

  int i = 0;
  while (is_in_range (k0, k1, r) && i <= (only_k1 ? 2 : 1) * r) {
    bool cl = (kl == k0 && only_k1) || is_falling (&kl->f, _bb, +0, +0);
    bool cr = (kr == k0 && only_k1) || is_falling (&kr->f, _bb, +0, +0);

    if (cl && cr) break;

    if (i++ % 2 && ! cl) move_frame (&kl->f, _bb, +2, -1, -1);
    else if (! cr) move_frame (&kr->f, _bb, +2, -1, -1);
  }
}

void
get_in_range (struct actor *k0, struct actor *k1, int r,
              bool only_k1, bool k1_dominant)
{
  struct actor *kd, *ks, *kl, *kr;

  kd = k1_dominant ? k1 : k0;
  ks = k1_dominant ? k0 : k1;

  kl = (kd->f.dir == LEFT) ? kd : ks;
  kr = (kd->f.dir == RIGHT) ? kd : ks;

  int i = 0;
  while (! is_in_range (k0, k1, r) && i <= (only_k1 ? 2 : 1) * r) {
    bool cl = (kl == k0 && only_k1) || is_falling (&kl->f, _bf, +0, +0);
    bool cr = (kr == k0 && only_k1) || is_falling (&kr->f, _bf, +0, +0);

    if (cl && cr) break;

    if (i++ % 2 && ! cl) move_frame (&kl->f, _bf, +2, +1, +1);
    else if (! cr) move_frame (&kr->f, _bf, +2, +1, +1);
  }
}

struct skill *
get_perfect_skill (struct skill *skill)
{
  skill->attack_prob = 99;
  skill->counter_attack_prob = 99;
  skill->defense_prob = 99;
  skill->counter_defense_prob = 99;
  skill->advance_prob = -1;
  skill->return_prob = 99;
  skill->refraction = 0;
  skill->extra_hp = INT_MAX - 6;
  return skill;
}

struct skill *
upgrade_skill (struct skill *s0, struct skill *s1, int total_hp)
{
  int ca = (s1->attack_prob + s1->counter_attack_prob) / 2;
  int cd = (s1->defense_prob + s1->counter_defense_prob) / 2;

  if (s0->counter_attack_prob < ca)
    s0->counter_attack_prob +=
      (ca - s0->counter_attack_prob) / (2 * total_hp);

  if (s0->counter_defense_prob < cd)
    s0->counter_defense_prob +=
      (cd - s0->counter_defense_prob) / (2 * total_hp);

  return s0;
}

void
alert_guards (struct pos *p)
{
  int i;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *g = &actor[i];
    if (is_guard (g) && is_pos_on_back (g, p)
        && g->current_hp > 0 && g->enemy_id == -1
        && anim_cycle - g->alert_cycle > 24) {
      invert_frame_dir (&g->f, &g->f);
      g->alert_cycle = anim_cycle;
    }
  }
}
