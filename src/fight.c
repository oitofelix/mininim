/*
  fight.c -- fight module;

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

#include "mininim.h"

bool
are_valid_opponents (struct anim *k0, struct anim *k1)
{
  /* no one fights oneself */
  if (k0->id == k1->id) return false;

  /* non-fightable characters can't fight */
  if (! is_fightable_anim (k0) || ! is_fightable_anim (k1))
    return false;

  /* non-fighters don't fight */
  if (! k0->fight || ! k1->fight)
    return false;

  /* anyone fightable trying to fight the kid is fair game */
  if (k0->id == 0 || k1->id == 0) return true;

  return false;
}

void
leave_fight_logic (struct anim *k)
{
  /* dead character doesn't fight */
  if (k->current_lives <= 0) {
    forget_enemy (k);
    return;
  }

  /* non-fightable characters don't fight */
  if (! is_fightable_anim (k)) return;

  /* controllables don't forget, they let this to the
     non-controllable */
  if (k->controllable) return;

  /* no enemy, no need to forget */
  if (k->enemy_id == -1) return;

  /* non-fighter doesn't fight */
  if (! k->fight) return;

  /* character that went upstairs doesn't fight */
  if (is_kid_stairs (&k->f)) {
    forget_enemy (k);
    return;
  }

  /* who's the enemy? */
  struct anim *ke = get_anim_by_id (k->enemy_id);

  /* if the enemy doesn't exist, forget about it */
  if (! ke) {
    forget_enemy (k);
    return;
  }

  /* if the enemy is dead no need to worry about him */
  if (ke->current_lives <= 0) {
    forget_enemy (k);
    return;
  }

  /* if the enemy went up stairs, forget about him */
  if (is_kid_stairs (&ke->f)) {
    forget_enemy (k);
    return;
  }

  /* if the enemy is not reachable, forget about him */
  enum dir odir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  struct pos p, pe;
  survey (_m, pos, &k->f, NULL, &p, NULL);
  survey (_m, pos, &ke->f, NULL, &pe, NULL);
  pos2room (&pe, p.room, &pe);
  if (! is_seeing (k, ke, k->f.dir)
      && ! is_near (k, ke)
      && ! is_safe_to_follow (k, ke, k->f.dir)
      && ! (is_on_back (k, ke) && is_seeing (k, ke, odir)
            && p.floor == pe.floor)) {
    forget_enemy (k);
    return;
  }
}

void
enter_fight_logic (struct anim *k)
{
  /* non-fightable characters don't fight */
  if (! is_fightable_anim (k)) return;

  /* has an enemy, no need to get another */
  if (k->enemy_id != -1 && k->enemy_aware) return;

  /* dead character doesn't fight */
  if (k->current_lives <= 0) return;

  /* non-fighter doesn't fight */
  if (! k->fight) return;

  int i;
  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];

    /* no dead character is a valid oponent */
    if (a->current_lives <= 0) continue;

    /* only valid opponents matter */
    if (! are_valid_opponents (k, a)) continue;

    /* if seeing the character consider him an enemy */
    if (is_seeing (k, a, k->f.dir)) {
      consider_enemy (k, a);
      return;
    }

    /* if hearing the character on the back, turn */
    if (! k->controllable && is_hearing (k, a)
        && is_on_back (k, a)
        && abs (anim_cycle - k->alert_cycle) > 24) {
      k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      k->alert_cycle = anim_cycle;
      return;
    }

    /* if feeling the character's presence consider him an enemy */
    if (is_near (k, a)) {
      consider_enemy (k, a);
      return;
    }
  }
}

void
fight_ai (struct anim *k)
{
  /* non-fightable characters don't fight */
  if (! is_fightable_anim (k)) return;

  /* controllables and non-fighters doesn't need AI to fight */
  if (k->controllable || ! k->fight) return;

  /* without an enemy or awareness, no need to fight */
  if (k->enemy_id == -1 || ! k->enemy_aware) {
    if (is_in_fight_mode (k)) leave_fight_mode (k);
    return;
  }

  /* first thing, enter in fight mode */
  if (! is_in_fight_mode (k)) {
    enter_fight_mode (k);
    return;
  }

  /* who's the enemy? */
  struct anim *ke = get_anim_by_id (k->enemy_id);

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
           && ! is_strictly_traversable (&k->enemy_pos))
    k->enemy_pos.place = pe.place;

  if (is_valid_pos (&k->enemy_pos)
      && is_strictly_traversable (&k->enemy_pos)) {
    struct pos pp;
    int d = (p.place < k->enemy_pos.place) ? -1 : +1;
    while (is_strictly_traversable (prel (&k->enemy_pos, &pp, +0, d)))
      k->enemy_pos = pp;
  }

  /* prevent enemy from passing through */
  if (ke->type == KID
      && is_near (k, ke)
      && ! is_in_fight_mode (ke)
      && ! ke->immortal
      && ke->f.dir != k->f.dir
      && ke->current_lives > 0
      && is_there_enough_room_to_fight (ke)) {
    place_on_the_ground (&ke->f, &ke->f.c);
    kid_take_sword (ke);
  }

  /* if the enemy is on the back, turn */
  if (is_on_back (k, ke) && is_seeing (k, ke, odir)
      && p.floor == pe.floor) {
    if (is_safe_to_turn (k)) fight_turn (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* if too near to a wall, back off to have room for an attack */
  if (fight_crel (k, +0, +1) == WALL
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
      && ke->f.dir != k->f.dir
      && ke->current_lives > 0
      && ((is_kid_run (&ke->f)
           && is_in_range (k, ke, 3 * PLACE_WIDTH - 4))
          || (is_kid_run_jump (&ke->f)
              && is_in_range (k, ke, 4 * PLACE_WIDTH - 4))
          || (is_kid_jump_air (&ke->f) && ke->i < 9
              && is_in_range (k, ke, 4 * PLACE_WIDTH)))) {
    if (is_safe_to_attack (k)) fight_attack (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* stays at least in the fight range.  Advance, unless the enemy is
     not running towards you */
  if (! is_in_range (k, ke, FIGHT_RANGE)
      && (ke->f.dir == k->f.dir
          || p.room != pe.room
          || p.floor != pe.floor
          || ! (is_kid_run (&ke->f)
                || is_kid_run_jump (&ke->f)
                || is_kid_jump (&ke->f)))
      && is_safe_to_follow (k, ke, k->f.dir)) {
    fight_walkf (k);
    return;
  }

  /* if the enemy is not targeting you, do nothing more */
  if (ke->enemy_id != -1 && ke->enemy_id != k->id) return;

  /* in fight range, if the enemy is not attacking, go towards attack
     range (with probability, unless the enemy is not in fight mode,
     then go immediately) */
  if (is_in_range (k, ke, FIGHT_RANGE)
      && ! is_in_range (k, ke, ATTACK_RANGE)
      && is_safe_to_follow (k, ke, k->f.dir)
      && (prandom (99) <= k->skill.advance_prob
          || ! is_in_fight_mode (ke))
      && ! is_attacking (ke)
      && (! is_in_fight_mode (ke)
          || is_walking (ke)
          || ke->i >= 6 + prandom (24))) {
    fight_walkf (k);
    return;
  }

  /* in attack range, back off to fight range
     (with probability) */
  if (is_in_range (k, ke, ATTACK_RANGE)
      && is_safe_to_walkb (k)
      && prandom (99) <= k->skill.return_prob) {
    fight_walkb (k);
    return;
  }

  /* in attack range, if being attacked, defend yourself (with
     probability) and counter attack (with probability handled
     elsewhere) */
  if (is_in_range (k, ke, ATTACK_RANGE)
      && (is_attacking (ke)
          && (k->type != KID || ke->i == 0)
          && (k->type == KID || ke->i == 1))
      && (prandom (99) <= k->skill.defense_prob
          || k->refraction > 0)
      && ke->current_lives > 0) {
    fight_defense (k);
    if (is_safe_to_attack (k)) fight_attack (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }

  /* if attacking, counter defend
     (with probability handled elsewhere) */
  if (is_in_range (k, ke, ATTACK_RANGE)
           && is_attacking (k)) {
    fight_defense (k);
    return;
  }

  /* in attack range, if not being attacked, attack (with probability,
     unless the enemy is not in fight mode, then attack immediately) */
  if (! is_attacking (ke)
      && ke->current_lives > 0
      && is_in_range (k, ke, ATTACK_RANGE)
      && (prandom (99) <= k->skill.attack_prob
          || ! is_in_fight_mode (ke))) {
    if (is_safe_to_attack (k)) fight_attack (k);
    else if (is_safe_to_walkb (k)) fight_walkb (k);
    return;
  }
}

void
fight_mechanics (struct anim *k)
{
  if (k->sword_immune > 0) k->sword_immune--;

  struct anim *ke = get_anim_by_id (k->enemy_id);

  if (! ke || ke->enemy_id != k->id) return;

  fight_inversion_mechanics (k, ke);

  if (! is_attacking (ke) || is_sword_hit (k)) return;

  if (! ke->attack_defended)
    ke->attack_defended = (ke->i < 4 && k->key.up);

  if (ke->attack_defended && (is_walkingf (k) || is_attacking (k)
                              || ! is_in_fight_mode (k)))
    ke->attack_defended = 0;

  if (! ke->counter_attacked)
    ke->counter_attacked =
      (ke->attack_defended && k->key.shift
       && prandom (99) <= k->skill.counter_attack_prob);

  if (! ke->counter_defense)
    ke->counter_defense =
      (ke->counter_attacked && ke->key.up
       && prandom (99) <= ke->skill.counter_defense_prob);

  if (! k->hurt_enemy_in_counter_attack)
    k->hurt_enemy_in_counter_attack = ke->counter_attacked && ! ke->counter_defense;

  /* printf ("ke->attack_defended = %i, ke->i = %i, k->key.up = %i\n", */
  /*         ke->attack_defended, ke->i, k->key.up); */

  if (! ke->attack_defended && is_at_hit_frame (ke)
      && is_in_range (k, ke, HIT_RANGE)) fight_hit (k, ke);
  else if (ke->hurt_enemy_in_counter_attack
           && is_at_hit_frame (ke)
           && is_in_range (k, ke, HIT_RANGE)) fight_hit (k, ke);
  else if (ke->attack_defended == 1 && is_at_defendable_attack_frame (ke)) {
    if (is_in_range (k, ke, HIT_RANGE + 4)) {
      put_at_defense_frame (k);
      put_at_attack_frame (ke);
    } else ke->attack_defended = 0;
  }

/*   printf ("id: %i, ad: %i, ca: %i, cd: %i, i: %i, hurt: %i\n\ */
/* id: %i, ad: %i, ca: %i, cd: %i, i: %i, hurt: %i\n\ */
/* -------------------------------\n", */
/*           k->id, k->attack_defended, k->counter_attacked, k->counter_defense, k->i, k->hurt, */
/*           ke->id, ke->attack_defended, ke->counter_attacked, ke->counter_defense, ke->i, ke->hurt); */
}

void
fight_inversion_mechanics (struct anim *k, struct anim *ke)
{
  if (is_in_range (k, ke, INVERSION_RANGE)
      && is_in_fight_mode (k)
      && is_in_fight_mode (ke)
      && ! is_sword_hit (k)
      && ! is_sword_hit (ke)
      && (! is_attacking (k) || k->i < 2)
      && (! is_attacking (ke) || ke->i < 2)) {
    struct coord c;
    c = k->f.c;
    k->f.c = ke->f.c;
    ke->f.c = c;

    place_on_the_ground (&k->f, &k->f.c);
    place_on_the_ground (&ke->f, &ke->f.c);

    fight_turn (k);
    fight_turn (ke);

    struct anim *kl = (k->f.dir == LEFT) ? k : ke;
    struct anim *kr = (k->f.dir == RIGHT) ? k : ke;

    int i = 0, j = 0;
    while (is_in_range (k, ke, INVERSION_RANGE)
           || j++ < 4) {
      if (i++ % 2) kl->f.c.x++;
      else kr->f.c.x--;
    }
  }
}

void
consider_enemy (struct anim *k0, struct anim *k1)
{
  k0->enemy_id = k1->id;
  k0->enemy_aware = true;
  k0->enemy_refraction = 12;
  k1->enemy_id = k0->id;
}

void
forget_enemy (struct anim *k)
{
  if (--k->enemy_refraction > 0) return;
  else k->enemy_refraction = 0;

  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (ke) {
    ke->oenemy_id = ke->enemy_id;
    ke->enemy_id = -1;
    ke->enemy_aware = false;
  }
  k->oenemy_id = k->enemy_id;
  k->enemy_id = -1;
  k->enemy_aware = false;
}

void
enter_fight_mode (struct anim *k)
{
  if (! k->has_sword) return;
  k->key.enter = true;
}

void
leave_fight_mode (struct anim *k)
{
  k->key.down = true;
}

bool
is_in_range (struct anim *k0, struct anim *k1, int r)
{
  struct coord m0, m1; struct pos p0, p1;
  survey (_m, pos, &k0->f, &m0, &p0, NULL);
  survey (_m, pos, &k1->f, &m1, &p1, NULL);
  coord2room (&m1, m0.room, &m1);

  return m1.room == m0.room
    && p1.floor == p0.floor
    && abs (m1.x - m0.x) < r;
}

enum confg
fight_crel (struct anim *k, int floor, int place)
{
  struct pos pm;
  survey (_m, pos, &k->f, NULL, &pm, NULL);

  /* place sign indicates direction in relation to k orientation */
  int dir = (k->f.dir == LEFT) ? -1 : +1;
  return crel (&pm, floor, dir * place)->fg;
}

int
dist_enemy (struct anim *k)
{
  struct anim *k1 = get_anim_by_id (k->enemy_id);

  if (! k1) return INT_MAX;

  struct frame f1 = k1->f;
  frame2room (&f1, k->f.c.room, &f1.c);

  struct coord m0, m1;
  _m (&k->f, &m0);
  _m (&f1, &m1);

  return abs (m1.x - m0.x);
}

bool
is_in_fight_mode (struct anim *k)
{
  return k->action == kid_sword_normal
    || k->action == kid_sword_walkf
    || k->action == kid_sword_walkb
    || k->action == kid_sword_attack
    || k->action == kid_sword_defense
    || k->action == kid_take_sword
    || k->action == kid_sword_hit

    || k->action == guard_vigilant
    || k->action == guard_walkf
    || k->action == guard_walkb
    || k->action == guard_attack
    || k->action == guard_defense
    || k->action == guard_hit;;
}

bool
is_attacking (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_attack
         || k->action == guard_attack;
}

bool
is_defending (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_defense
         || k->action == guard_defense;
}

bool
is_walking (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_walkf
         || k->action == kid_sword_walkb
         || k->action == guard_walkf
         || k->action == guard_walkb;
}

bool
is_walkingf (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_walkf
         || k->action == guard_walkf;
}

bool
is_sword_hit (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_hit
         || k->action == guard_hit;
}

bool
is_at_defendable_attack_frame (struct anim *k)
{
  return k->fo.b == kid_sword_attack_02
    || k->fo.b == kid_sword_attack_03
    || k->fo.b == guard_attack_03
    || k->fo.b == guard_attack_04
    || k->fo.b == fat_guard_attack_03
    || k->fo.b == fat_guard_attack_04
    || k->fo.b == vizier_attack_03
    || k->fo.b == vizier_attack_04
    || k->fo.b == skeleton_attack_03
    || k->fo.b == skeleton_attack_04
    || k->fo.b == shadow_attack_03
    || k->fo.b == shadow_attack_04;
}

bool
is_at_hit_frame (struct anim *k)
{
  return k->fo.b == kid_sword_attack_03
    || k->fo.b == guard_attack_04
    || k->fo.b == fat_guard_attack_04
    || k->fo.b == vizier_attack_04
    || k->fo.b == skeleton_attack_04
    || k->fo.b == shadow_attack_04;
}

void
put_at_defense_frame (struct anim *k)
{
  struct frameset *frameset;
  play_sample (sword_defense_sample, NULL, k->id);

  switch (k->type) {
  case NO_ANIM: default: break;
  case KID:
    select_frame (k, kid_sword_defense_frameset, 0);
    next_frame (&k->f, &k->f, &k->fo);

    select_frame (k, kid_sword_defense_frameset, 1);

    struct anim *ke = get_anim_by_id (k->enemy_id);
    if (ke->type == KID) {
      select_xframe (&k->xf, sword_frameset, 11);
      k->xf.dx = -13;
      k->xf.dy = +5;
    } else select_xframe (&k->xf, sword_frameset, 14);

    k->action = kid_sword_defense;
    next_frame (&k->f, &k->f, &k->fo);
    break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    frameset = get_guard_defense_frameset (k->type);
    select_frame (k, frameset, 0);
    select_xframe (&k->xf, sword_frameset, 11);
    k->action = guard_defense;
    next_frame (&k->f, &k->f, &k->fo);
    break;
  }

  /* if (k->id == 0) */
  /*   printf ("%s: k->i = %i, k->fo.dx = %i\n", */
  /*           __func__, k->i, k->fo.dx); */
}

void
put_at_attack_frame (struct anim *k)
{
  k->attack_defended = 2;
  k->counter_attacked = k->counter_attacked ? 2 : 0;
  k->counter_defense = k->counter_defense ? 2 : 0;

  switch (k->type) {
  case NO_ANIM: default: break;
  case KID:
    if (k->i == 3) return;

    k->f = k->of;
    select_frame (k, kid_sword_attack_frameset, 0);
    next_frame (&k->f, &k->f, &k->fo);
    select_frame (k, kid_sword_attack_frameset, 1);
    next_frame (&k->f, &k->f, &k->fo);
    select_frame (k, kid_sword_attack_frameset, 2);

    k->fo.b = kid_sword_attack_defended;
    select_xframe (&k->xf, sword_frameset, 17);
    k->xf.dx = -21;
    k->xf.dy = +11;
    next_frame (&k->f, &k->f, &k->fo);
    break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    k->fo.b = get_guard_attack_defended_bitmap (k->type);
    k->fo.dx = +1;
    k->fo.dy = 0;

    select_xframe (&k->xf, sword_frameset, 8);
    k->xf.dx = -13;
    k->xf.dy = -14;
    next_frame (&k->f, &k->f, &k->fo);
    break;
  }

}

bool
opaque_cs (enum confg t)
{
  return t == WALL || t == CARPET || t == TCARPET || t == MIRROR;
}

bool
is_opaque_at_left (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == WALL || t == MIRROR;
}

bool
is_opaque_at_right (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == WALL || t == CARPET || t == TCARPET;
}

bool
is_seeing (struct anim *k0, struct anim *k1, enum dir dir)
{
  struct coord m0, m1, mt1, mb1; struct pos p, p0, p1, pk, pke;
  survey (_m, pos, &k0->f, &m0, &p0, NULL);

  coord_f cf;
  if (is_kid_climb (&k1->f) || is_anim_fall (&k1->f)) {
    coord2room (_mt (&k1->f, &mt1), m0.room, &mt1);
    coord2room (_m (&k1->f, &m1), m0.room, &m1);
    coord2room (_mbo (&k1->f, &mb1), m0.room, &mb1);

    double dt, dm, db;
    dt = (mt1.room == m0.room) ? dist_coord (&m0, &mt1) : INFINITY;
    dm = (m1.room == m0.room) ? dist_coord (&m0, &m1) : INFINITY;
    db = (mb1.room == m0.room) ? dist_coord (&m0, &mb1) : INFINITY;

    if (dt <= dm && dt <= db)
      cf = (k0->f.dir == LEFT) ? _tr : _tl;
    else if (db <= dt && db <= dm)
      cf = (k0->f.dir == LEFT) ? _br : _bl;
    else cf = (k0->f.dir == LEFT) ? _mr : _ml;
  } else cf = (k0->f.dir == LEFT) ? _mr : _ml;

  survey (cf, pos, &k1->f, &m1, NULL, NULL);

  coord2room (&m0, p0.room, &m0);
  coord2room (&m1, p0.room, &m1);
  pos (&m1, &p1);

  if (dir == LEFT) {
    if (is_opaque_at_left (&p0)) return false;
    if (is_opaque_at_right (&p1)) return false;
    if (peqr (&p1, &p0, +0, -1)) return true;
    prel (&p0, &pk, +0, -1);
    prel (&p1, &pke, +0, +1);
  } else {
    if (is_opaque_at_right (&p0)) return false;
    if (is_opaque_at_left (&p1)) return false;
    if (peqr (&p1, &p0, +0, +1)) return true;
    prel (&p0, &pk, +0, +1);
    prel (&p1, &pke, +0, -1);
  }

  if (peq (&p0, &p1)) return true;

  first_confg (&pk, &pke, opaque_cs, &p);

  return p0.room == p1.room
    && m1.room == m0.room
    && p1.floor == p0.floor
    && ! (dir == LEFT && m1.x > m0.x)
    && ! (dir == RIGHT && m1.x < m0.x)
    && p.room == -1;
}

bool
is_hearing (struct anim *k0, struct anim *k1)
{
  struct pos p0, p1;
  survey (_m, pos, &k0->f, NULL, &p0, NULL);
  survey ((k0->f.dir == LEFT) ? _mr : _ml,
          pos, &k1->f, NULL, &p1, NULL);

  pos2room (&p1, p0.room, &p1);

  return p1.room == p0.room
    && (is_kid_run (&k1->f)
        || is_kid_stop_run (&k1->f)
        || is_kid_jump_landing (&k1->f)
        || is_kid_run_jump_running (&k1->f)
        || is_kid_run_jump_landing (&k1->f)
        || (is_kid_couch (&k1->f)
            && k1->fall)
        || k1->action == kid_take_sword);
}

bool
is_pos_on_back (struct anim *k, struct pos *p)
{
  struct pos pm, pv;
  survey (_m, pos, &k->f, NULL, &pm, NULL);

  pos2room (p, pm.room, &pv);

  return pm.room == pv.room
    && ((k->f.dir == LEFT && pv.place > pm.place)
        || (k->f.dir == RIGHT && pv.place < pm.place));
}

bool
is_on_back (struct anim *k0, struct anim *k1)
{
  struct coord m0, m1;

  _m (&k0->f, &m0);
  _m (&k1->f, &m1);
  coord2room (&m1, m0.room, &m1);

  return m1.room == m0.room
    && ((k0->f.dir == LEFT && m1.x > m0.x)
        || (k0->f.dir == RIGHT && m1.x < m0.x));
}

bool
is_near (struct anim *k0, struct anim *k1)
{
  struct coord m0, m1; struct pos p0, p1;
  survey (_m, pos, &k0->f, &m0, &p0, NULL);
  survey (_m, pos, &k1->f, &m1, &p1, NULL);

  _m (&k0->f, &m0);
  _m (&k1->f, &m1);
  coord2room (&m1, m0.room, &m1);

  return m1.room == m0.room && p1.floor == p0.floor
    && abs (m1.x - m0.x) < PLACE_WIDTH
    && abs (m1.y - m0.y) < PLACE_HEIGHT;
}

bool
is_safe_to_walkb (struct anim *k)
{
  int df = dist_fall (&k->f, true);
  return (df > PLACE_WIDTH) && is_there_enough_room_to_fight (k);
}

bool
is_safe_to_attack (struct anim *k)
{
  int df = dist_fall (&k->f, false);
  return df > PLACE_WIDTH && is_there_enough_room_to_fight (k);
}

bool
is_safe_to_turn (struct anim *k)
{
  int df = dist_fall (&k->f, false);
  return (df > PLACE_WIDTH);
}

bool
dangerous_cs (enum confg t)
{
  return traversable_cs (t)
    || t == WALL || t == CARPET || t == TCARPET || t == MIRROR || t == CHOPPER;
}

bool
door_cs (enum confg t)
{
  return t == DOOR;
}

bool
is_safe_at_right (struct pos *p, struct frame *f)
{
  enum confg t = con (p)->fg;

  struct coord tf;
  _tf (f, &tf);

  if (t == DOOR)
   return tf.y > door_grid_tip_y (p) - 10
     || door_at_pos (p)->action == OPEN_DOOR;

  return t != WALL && t != CARPET && t != TCARPET;
}

bool
is_safe_at_left (struct pos *p)
{
  enum confg t = con (p)->fg;

  return t != WALL && t != CHOPPER && t != MIRROR;
}

bool
is_safe_to_follow (struct anim *k0, struct anim *k1, enum dir dir)
{
  /* not aware of enemy position */
  if (! is_valid_pos (&k0->enemy_pos)) return false;

  struct coord tf; struct pos p0, p, p1, pke, pk;
  survey (_m, pos, &k0->f, NULL, &p0, NULL);
  survey ((k0->f.dir == LEFT) ? _mr : _ml,
          pos, &k1->f, NULL, &p1, NULL);
  _tf (&k0->f, &tf);

  pos2room (&p1, p0.room, &p1);

  /* not reachable, to start with */
  if (p0.room != p1.room) return false;
  if (p0.floor > p1.floor) return false;

  /* the character went down after the enemy */
  if (p0.floor != k0->enemy_pos.floor) {
    /* enum dir odir = (dir == LEFT) ? RIGHT : LEFT; */
    /* if (is_on_back (k0, k1) && is_seeing (k0, k1, odir) */
    /*     && p0.floor == p1.floor) return true; */
    if (is_seeing (k0, k1, LEFT)
        || is_seeing (k0, k1, RIGHT)) return true;
    return false;
  }

  /* if falling the chasing is inevitable (necessary to prevent
     leave_fight_logic from forgeting enemy based on the facing
     direction of the is_in_range check) */
  if (is_anim_fall (&k0->f)) return true;

  if (dir == LEFT) {
    if (! is_safe_at_left (&p0)) return false;
    if (! is_safe_at_right (&k0->enemy_pos, &k0->f)) return false;
    if (peqr (&k0->enemy_pos, &p0, +0, -1)) return true;
    prel (&p0, &pk, +0, -1);
    prel (&k0->enemy_pos, &pke, +0, +1);
    if (con (&pk)->fg == DOOR
        && tf.y <= door_grid_tip_y (&pk) - 10)
      return false;
  } else {
    if (! is_safe_at_right (&p0, &k0->f)) return false;
    if (! is_safe_at_left (&k0->enemy_pos)) return false;
    if (peqr (&k0->enemy_pos, &p0, +0, +1)) return true;
    prel (&p0, &pk, +0, +1);
    prel (&k0->enemy_pos, &pke, +0, -1);
    if (con (&p0)->fg == DOOR
        && tf.y <= door_grid_tip_y (&p0) - 10)
      return false;
  }

  /* enemy went down */
  if (is_traversable (&k0->enemy_pos)) {
    prel (&k0->enemy_pos, &pke, +1, +0);
    if (is_traversable (&pke) || con (&pke)->fg == SPIKES_FLOOR)
      return false;
    int d = (dir == LEFT) ? -1 : +1;
    if (peq (&pk, &k0->enemy_pos)) return true;
    prel (&k0->enemy_pos, &pke, +0, -1 * d);
  } else if (peq (&p0, &k0->enemy_pos)
             && ! (is_seeing (k0, k1, LEFT)
                   || is_seeing (k0, k1, RIGHT)))
    return false;

  first_confg (&pk, &pke, dangerous_cs, &p);
  if (is_valid_pos (&p)) return false;

  first_confg (&pk, &pke, door_cs, &p);
  if (! is_valid_pos (&p)) return true;
  else return door_at_pos (&p)->action == OPEN_DOOR
         || tf.y > door_grid_tip_y (&p) - 10;
}

bool
is_there_enough_room_to_fight (struct anim *k)
{
  return dist_collision (&k->f, false, &k->ci) > PLACE_WIDTH
    || dist_collision (&k->f, true, &k->ci) > PLACE_WIDTH;
}

void
fight_turn (struct anim *k)
{
  k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  k->f.flip ^= ALLEGRO_FLIP_HORIZONTAL;

  if (! is_in_fight_mode (k)
      && is_there_enough_room_to_fight (k)) enter_fight_mode (k);
  else {
    struct pos p;
    struct anim a = *k;
    anim_walkb (&a);
    survey (_bb, pos, &a.f, NULL, &p, NULL);
    if (! is_strictly_traversable (&p)
        && is_there_enough_room_to_fight (k)) *k = a;
  }
}

void
fight_turn_controllable (struct anim *k)
{
  int d = INT_MAX;
  int t; /* threshold */
  struct anim *ke = NULL;
  struct anim *ke0 = get_anim_by_id (k->enemy_id);

  /* make the kid target the nearest enemy targeting him */
  int i;
  for (i = 0; i < anima_nmemb; i++) {
    struct anim *a = &anima[i];
    if (a->enemy_id != k->id || ! is_fightable_anim (a))
      continue;
    int de = dist_enemy (a);
    if (de < d) {
      d = de;
      ke = a;
    }
  }

  if (ke && ke0 && ke->f.dir != ke0->f.dir) t = 20;
  else t = -1;

  if (ke && abs (dist_enemy (k) - d) > t)
    consider_enemy (k, ke);

  ke = get_anim_by_id (k->enemy_id);
  if (ke) {
    struct pos p, pe;
    survey (_m, pos, &k->f, NULL, &p, NULL);
    survey (_m, pos, &ke->f, NULL, &pe, NULL);
    if (is_on_back (k, ke) && is_in_fight_mode (k)
        && p.floor == pe.floor)
      fight_turn (k);
  }
}

void
fight_defense (struct anim *k)
{
  k->key.up = true;
}

void
fight_attack (struct anim *k)
{
  k->key.shift = true;
}

void
fight_walkf (struct anim *k)
{
  if (k->f.dir == LEFT) k->key.left = true;
  else k->key.right = true;
}

void
fight_walkb (struct anim *k)
{
  if (k->f.dir == LEFT) k->key.right = true;
  else k->key.left = true;
}

void
fight_hit (struct anim *k, struct anim *ke)
{
  if (k->immortal || k->sword_immune) return;
  if (k->current_lives <= 0) return;

  place_on_the_ground (&k->f, &k->f.c);
  k->xf.b = NULL;

  k->current_lives--;

  /* int d = (k->f.dir == LEFT) ? +1 : -1; */
  struct pos pb;
  survey (_bb, pos, &k->f, NULL, &k->p, NULL);

  /* ensure anim doesn't die within a wall */
  if (con (&k->p)->fg == WALL) {
    if (crel (&k->p, +0, +1)->fg != WALL) k->p.place++;
    else if (crel (&k->p, +0, -1)->fg != WALL) k->p.place--;
  }

  /* prel (&k->p, &pb, 0, d); */
  pb = k->p;

  if (! is_colliding (&k->f, &k->fo, +PLACE_WIDTH,
                      ke->f.dir != k->f.dir, &k->ci)) {
    if (is_strictly_traversable (&pb)) {
      place_at_pos (&k->f, _m, &pb, &k->f.c);
      anim_fall (k);
    } else if (is_in_range (ke, k, PLACE_WIDTH))
      place_at_distance (&ke->f, _tf, &k->f, _tf, +0,
                         ke->f.dir, &k->f.c);
    else place_at_distance (&ke->f, _tf, &k->f, _tf, +10,
                            ke->f.dir, &k->f.c);
  }

  if (k->current_lives <= 0 || ! is_in_fight_mode (k)) {
    forget_enemy (ke);
    if (! is_anim_fall (&k->f)) anim_die (k);
    k->death_reason = FIGHT_DEATH;
    ke->alert_cycle = anim_cycle;
  } else if (! is_anim_fall (&k->f)) anim_sword_hit (k);

  k->splash = true;

  if (k->id == current_kid_id) {
    mr.flicker = 2;
    mr.color = get_flicker_blood_color ();
    play_sample (harm_sample, NULL, k->id);
  } else play_sample (guard_hit_sample, NULL, k->id);
}

bool
fight_door_split_collision (struct anim *a)
{
  struct coord tl; struct pos ptl, ptr;

  survey (_tl, pos, &a->f, &tl, &ptl, NULL);
  survey (_tr, pos, &a->f, NULL, &ptr, NULL);

  int dtl = dist_next_place (&a->f, _tl, pos, +0, a->f.dir == LEFT);
  int dtr = dist_next_place (&a->f, _tr, pos, +0, a->f.dir == RIGHT);

  if (con (&ptl)->fg == DOOR
      && ptr.place > ptl.place
      && tl.y <= door_grid_tip_y (&ptl) - 10) {
    if (dtl < dtr) {
     a->f.c.x += dtl;
      if (a->f.dir == RIGHT) anim_walkf (a);
      else anim_walkb (a);
      return true;
    } else {
      a->f.c.x -= dtr;
      if (a->f.dir == LEFT) anim_walkf (a);
      else anim_walkb (a);
      return true;
    }
  }

  return false;
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
  skill->extra_life = INT_MAX - 6;
  return skill;
}

struct skill *
upgrade_skill (struct skill *s0, struct skill *s1)
{
  int ca = (s1->attack_prob + s1->counter_attack_prob) / 2;
  int cd = (s1->defense_prob + s1->counter_defense_prob) / 2;

  if (s0->counter_attack_prob < ca)
    s0->counter_attack_prob = (s0->counter_attack_prob + ca) / 2;
  else if (s0->counter_attack_prob < 99)
    s0->counter_attack_prob += 1;

  if (s0->counter_defense_prob < cd)
    s0->counter_defense_prob = (s0->counter_defense_prob + cd) / 2;
  else if (s0->counter_defense_prob < 99)
    s0->counter_defense_prob += 1;

  return s0;
}

void
alert_guards (struct pos *p)
{
  int i;
  for (i = 0; i < anima_nmemb; i++) {
    struct anim *g = &anima[i];
    if (is_guard (g) && is_pos_on_back (g, p)
        && g->current_lives > 0 && g->enemy_id == -1
        && abs (anim_cycle - g->alert_cycle) > 24) {
      g->f.dir = (g->f.dir == LEFT) ? RIGHT : LEFT;
      g->f.flip ^= ALLEGRO_FLIP_HORIZONTAL;
      g->alert_cycle = anim_cycle;
    }
  }
}
