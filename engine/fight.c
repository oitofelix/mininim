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

#include <stdio.h>
#include "kernel/video.h"
#include "kernel/random.h"
#include "kid/kid.h"
#include "guard/guard.h"
#include "anim.h"
#include "physics.h"
#include "pos.h"
#include "mouse.h"
#include "sword.h"
#include "fight.h"

void
fight_ai (struct anim *k0, struct anim *k1)
{
  if (k0->current_lives <= 0) return;

  /* in attack range, if being attacked, defend yourself (with
     probability) and counter attack (with probability handled
     elsewhere)*/
  if (is_in_fight_mode (k0)
      && k0->enemy_id == k1->id
      && ! is_on_back (k0, k1)
      && is_in_range (k0, k1, ATTACK_RANGE)
      && (is_attacking (k1)
          && (k0->type != KID || k1->i == 0)
          && (k0->type == KID || k1->i == 1))
      && prandom (99) <= k0->skill.defense_prob) {
    fight_defense (k0);
    if (is_safe_to_attack (k0)) fight_attack (k0);
    else if (is_safe_to_walkb (k0)) fight_walkb (k0);
  }
  /* if defending, counter attack
     (with probability handled elsewhere) */
  /* else if (is_in_fight_mode (k0) */
  /*          && k0->enemy_id == k1->id */
  /*          && ! is_on_back (k0, k1) */
  /*          && is_in_range (k0, k1, ATTACK_RANGE) */
  /*          && is_defending (k0)) { */
  /*   if (is_safe_to_attack (k0)) fight_attack (k0); */
  /*   else if (is_safe_to_walkb (k0)) fight_walkb (k0); */
  /* } */
  /* if attacking, counter defend
     (with probability handled elsewhere) */
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && ! is_on_back (k0, k1)
           && is_in_range (k0, k1, ATTACK_RANGE)
           && is_attacking (k0))
    fight_defense (k0);
  /* in attack range, if not being attacked, attack (with probability,
     unless the enemy is not in fight mode, then attack immediately) */
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && ! is_on_back (k0, k1)
           && ! is_attacking (k1)
           && is_in_range (k0, k1, ATTACK_RANGE)
           && (prandom (99) <= k0->skill.attack_prob
               || ! is_in_fight_mode (k1))) {
    if (is_safe_to_attack (k0)) fight_attack (k0);
    else if (is_safe_to_walkb (k0)) fight_walkb (k0);
  }
   /* in attack range, back off to fight range
      (with probability) */
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && ! is_attacking (k1)
           && is_in_range (k0, k1, ATTACK_RANGE)
           && is_safe_to_walkb (k0)
           && prandom (99) <= k0->skill.return_prob)
    fight_walkb (k0);
  /* in fight range, go towards attack range
     (with probability) */
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && ! is_attacking (k1)
           && is_in_range (k0, k1, FIGHT_RANGE)
           && ! is_in_range (k0, k1, ATTACK_RANGE)
           && is_safe_to_walkf (k0)
           && prandom (99) <= k0->skill.advance_prob)
    fight_walkf (k0);
  /* in follow range, stays at least in the fight range */
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && ! is_in_range (k0, k1, FIGHT_RANGE)
           && is_in_range (k0, k1, FOLLOW_RANGE)
           && ! is_on_back (k0, k1)
           && is_safe_to_walkf (k0))
    fight_walkf (k0);
  /* in follow range, but enemy on back, turn if safe*/
  else if (is_in_fight_mode (k0)
           && k0->enemy_id == k1->id
           && is_in_range (k0, k1, FOLLOW_RANGE)
           && is_on_back (k0, k1)) {
    if (is_safe_to_turn (k0)) fight_turn (k0);
    else if (is_safe_to_walkb (k0)) fight_walkb (k0);
  } else if (is_seeing (k0, k1)) {
    if (k0->enemy_id != k1->id
        && k1->current_lives > 0)
      consider_enemy (k0, k1);
    if (k0->has_sword
        && ! is_in_fight_mode (k0))
      enter_fight_mode (k0);
  } else if ((is_hearing (k0, k1)
              || is_near (k0, k1))
             && is_on_back (k0, k1)
             && k0->enemy_id != k1->id) {
    consider_enemy (k0, k1);
    fight_turn (k0);
  } else if (is_in_fight_mode (k0)
             && ! (k0->enemy_id == k1->id
                   && is_in_range (k0, k1, FIGHT_RANGE))) {
    forget_enemy (k0);
    leave_fight_mode (k0);
  }

}

void
fight_mechanics (struct anim *k)
{
  struct anim *ke = get_anim_by_id (k->enemy_id);

  if (! ke) return;

  fight_inversion_mechanics (k, ke);

  if (! is_attacking (ke) || is_sword_hit (k)) return;

  if (! ke->attack_defended)
    ke->attack_defended = (ke->i < 4 && k->key.up);

  if (ke->attack_defended && (is_walking (k) || is_attacking (k)
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
    put_at_defense_frame (k);
    put_at_attack_frame (ke);
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
      && ! is_sword_hit (ke)) {
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

    int i = 0;
    while (is_in_range (k, ke, INVERSION_RANGE)) {
      if (i++ % 2) kl->f.c.x++;
      else kr->f.c.x--;
    }
  }
}

void
consider_enemy (struct anim *k0, struct anim *k1)
{
  k0->enemy_id = k1->id;
  k1->enemy_id = k0->id;
}

void
forget_enemy (struct anim *k)
{
  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (ke) ke->enemy_id = -1;
  k->enemy_id = -1;
}

void
enter_fight_mode (struct anim *k)
{
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
  struct frame f1 = k1->f;
  frame2room (&f1, k0->f.c.room, &f1.c);

  struct coord m0, m1; struct pos np, pm0, pm1;
  survey (_m, pos, &k0->f, &m0, &pm0, &np);
  survey (_m, pos, &f1, &m1, &pm1, &np);

  return k0->f.c.room == f1.c.room
    && pm1.floor == pm0.floor
    && abs (m1.x - m0.x) < r;
}

int
dist_enemy (struct anim *k)
{
  struct anim *k1 = get_anim_by_id (k->enemy_id);

  if (! k1) return INT_MAX;

  struct frame f1 = k1->f;
  frame2room (&f1, k->f.c.room, &f1.c);

  struct coord m0, m1; struct pos np, pm0, pm1;
  survey (_m, pos, &k->f, &m0, &pm0, &np);
  survey (_m, pos, &f1, &m1, &pm1, &np);

  return abs (m1.x - m0.x);
}

/* void */
/* fight_follow (struct anim *k0, struct anim *k1) */
/* { */
/*   struct frame f1 = k1->f; */
/*   frame2room (&f1, k0->f.c.room, &f1.c); */

/*   struct coord m0, m1; struct pos np, pm0, pm1; */
/*   survey (_m, pos, &k0->f, &m0, &pm0, &np); */
/*   survey (_m, pos, &f1, &m1, &pm1, &np); */

/*   if (abs (m1.x - m0.x) > FIGHT_RANGE) { */
/*     if (m1.x < m0.x && k0->f.dir == LEFT) k0->key.left = true; */
/*     else if (m1.x < m0.x && k0->f.dir == RIGHT) fight_turn (k0); */
/*     else if (m1.x > m0.x && k0->f.dir == LEFT) fight_turn (k0); */
/*     else if (m1.x > m0.x && k0->f.dir == RIGHT) k0->key.right = true; */
/*   } */
/*   else if (abs (m1.x - m0.x) <= MIN_FIGHT_DISTANCE) { */
/*     if (m1.x < m0.x && k0->f.dir == LEFT) k0->key.right = true; */
/*     else if (m1.x < m0.x && k0->f.dir == RIGHT) fight_turn (k0); */
/*     else if (m1.x > m0.x && k0->f.dir == LEFT) fight_turn (k0); */
/*     else if (m1.x > m0.x && k0->f.dir == RIGHT) k0->key.left = true; */
/*   } */
/* } */

bool
is_in_fight_mode (struct anim *k)
{
  return k->action == kid_sword_normal
    || k->action == kid_sword_walkf
    || k->action == kid_sword_walkb
    || k->action == kid_sword_attack
    || k->action == kid_sword_defense
    || k->action == kid_take_sword

    || k->action == guard_vigilant
    || k->action == guard_walkf
    || k->action == guard_walkb
    || k->action == guard_attack
    || k->action == guard_defense;
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
is_sword_hit (struct anim *k)
{
  if (! k) return false;
  else return k->action == kid_sword_hit
         || k->action == guard_hit;
}

bool
is_at_defendable_attack_frame (struct anim *k)
{
  return k->fo.b == kid_sword_attack_03
    || k->fo.b == kid_sword_attack_04
    || k->fo.b == guard_attack_04
    || k->fo.b == guard_attack_05
    || k->fo.b == fat_guard_attack_04
    || k->fo.b == fat_guard_attack_05
    || k->fo.b == vizier_attack_04
    || k->fo.b == vizier_attack_05
    || k->fo.b == skeleton_attack_04
    || k->fo.b == skeleton_attack_05;
}

bool
is_at_hit_frame (struct anim *k)
{
  return k->fo.b == kid_sword_attack_04
    || k->fo.b == guard_attack_05
    || k->fo.b == fat_guard_attack_05
    || k->fo.b == vizier_attack_05
    || k->fo.b == skeleton_attack_05;
}

void
put_at_defense_frame (struct anim *k)
{
  struct frameset *frameset;
  play_sample (sword_defense_sample, k->f.c.room);

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

    k->fo.b = kid_sword_attack_16;
    select_xframe (&k->xf, sword_frameset, 17);
    k->xf.dx = -21;
    k->xf.dy = +11;
    next_frame (&k->f, &k->f, &k->fo);
    break;
  case GUARD:
  case FAT_GUARD:
  case VIZIER:
  case SKELETON:
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
is_seeing (struct anim *k0, struct anim *k1)
{
  struct coord m0, m1; struct pos np, pm0, pm1;
  survey (_m, pos, &k0->f, &m0, &pm0, &np);
  survey (_m, pos, &k1->f, &m1, &pm1, &np);
  return pm1.room == pm0.room && pm1.floor == pm0.floor
    && ! (k0->f.dir == LEFT && m1.x > m0.x)
    && ! (k0->f.dir == RIGHT && m1.x < m0.x);
}

bool
is_hearing (struct anim *k0, struct anim *k1)
{
  struct coord nc; struct pos np, pm0, pm1;
  survey (_m, pos, &k0->f, &nc, &pm0, &np);
  survey (_m, pos, &k1->f, &nc, &pm1, &np);
  return pm1.room == pm0.room
    && (is_kid_run (&k1->f)
        || is_kid_stop_run (&k1->f)
        || is_kid_jump_landing (&k1->f)
        || is_kid_run_jump_running (&k1->f)
        || is_kid_run_jump_landing (&k1->f)
        || (is_kid_couch (&k1->f)
            && k1->fall));
}

bool
is_on_back (struct anim *k0, struct anim *k1)
{
  struct coord m0, m1; struct pos np, pm0, pm1;
  survey (_m, pos, &k0->f, &m0, &pm0, &np);
  survey (_m, pos, &k1->f, &m1, &pm1, &np);
  return pm1.room == pm0.room
    && ((k0->f.dir == LEFT && m1.x > m0.x)
        || (k0->f.dir == RIGHT && m1.x < m0.x));
}

bool
is_near (struct anim *k0, struct anim *k1)
{
  struct coord m0, m1; struct pos np, pm0, pm1;
  survey (_m, pos, &k0->f, &m0, &pm0, &np);
  survey (_m, pos, &k1->f, &m1, &pm1, &np);
  return pm1.room == pm0.room
    && ((k0->f.dir == LEFT && abs (m1.x - m0.x) < PLACE_WIDTH)
        || (k0->f.dir == RIGHT && abs (m1.x - m0.x) < PLACE_WIDTH));
}

bool
is_safe_to_walkf (struct anim *k)
{
  int df = dist_fall (&k->f, false);
  return (df > PLACE_WIDTH);
}

bool
is_safe_to_walkb (struct anim *k)
{
  int df = dist_fall (&k->f, true);
  return (df > PLACE_WIDTH);
}

bool
is_safe_to_attack (struct anim *k)
{
  int df = dist_fall (&k->f, false);
  return (df > PLACE_WIDTH);
}

bool
is_safe_to_turn (struct anim *k)
{
  int df = dist_fall (&k->f, false);
  return (df > PLACE_WIDTH);
}

void
fight_turn (struct anim *k)
{
  k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  if (! is_in_fight_mode (k)) enter_fight_mode (k);
  else anim_walkb (k);
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

  place_on_the_ground (&k->f, &k->f.c);
  k->xf.b = NULL;

  k->current_lives--;

  int d = (k->f.dir == LEFT) ? +1 : -1;
  struct coord nc; struct pos np, pb;
  survey (_m, pos, &k->f, &nc, &k->p, &np);
  prel (&k->p, &pb, 0, d);

  if (k->current_lives <= 0 || ! is_in_fight_mode (k)) {
    if (ke->id == 0) play_sample (glory_sample, ke->f.c.room);
    forget_enemy (ke);
    anim_die (k);
  } else anim_sword_hit (k);

  if (is_strictly_traversable (&pb)) {
    place_at_pos (&k->f, _m, &pb, &k->f.c);
    anim_fall (k);
  } else place_at_distance (&ke->f, _tf, &k->f, _tf, 10, ke->f.dir, &k->f.c);

  k->splash = true;

  if (k->type == KID) {
    video_effect.color = get_flicker_blood_color ();
    start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.1));
    play_sample (sword_hit_sample, k->f.c.room);
  } else play_sample (guard_hit_sample, k->f.c.room);
}

struct skill *
get_perfect_skill (struct skill *skill)
{
  skill->attack_prob = 80;
  skill->defense_prob = 99;
  skill->counter_attack_prob = 99;
  skill->counter_defense_prob = 99;
  skill->advance_prob = -1;
  skill->return_prob = 99;
  return skill;
}

struct skill *
get_median_skill (struct skill *skill)
{
  skill->attack_prob = 5;
  skill->defense_prob = 50;
  skill->counter_attack_prob = 50;
  skill->counter_defense_prob = 50;
  skill->advance_prob = 5;
  skill->return_prob = -1;
  return skill;
}
