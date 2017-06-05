/*
  kid-sword-normal.c -- kid sword normal module;

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_sword_normal (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_sword_normal;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  struct pos pmt;

  bool defense = k->key.up && ! k->key.shift
    && ! k->key.left && ! k->key.right
    && ! k->key.ctrl && ! k->key.alt
    && k->enemy_id != -1
    && k->enemy_refraction < 0;
  bool attack = k->key.shift && ! k->key.up
    && ! k->key.left && ! k->key.right
    && ! k->key.ctrl && ! k->key.alt
    && k->enemy_id != -1
    && k->enemy_refraction < 0;
  bool walkf = ! k->no_walkf_timer
    && (((k->f.dir == RIGHT) && k->key.right)
        || ((k->f.dir == LEFT) && k->key.left))
    && ! k->key.ctrl;
  bool walkb =
    (((k->f.dir == RIGHT) && k->key.left)
     || ((k->f.dir == LEFT) && k->key.right))
    && ! k->key.ctrl;

  k->enemy_defended_my_attack = 0;
  k->enemy_counter_attacked_myself = 0;
  k->i_counter_defended = 0;
  k->hurt_enemy_in_counter_attack = false;
  k->attack_range_far = false;
  k->attack_range_near = false;

  struct actor *ke = get_reciprocal_enemy (k);
  k->keep_sword_fast = (ke && ke->current_hp > 0
                        && ! is_actor_fall (ke));

  bool keep_sword =
    (k->key.down && (! ke || ! is_in_range (k, ke, ATTACK_RANGE)))
    || (k->auto_taken_sword && k->enemy_id == -1);

  if (k->oaction != kid_sword_normal) {
    k->i = -1;
    k->i_initiated_attack = false;
  }

  if (k->oaction == kid_sword_normal
      && k->current_hp <= 0) {
    survey (_mt, pos, &k->f, NULL, &pmt, NULL);
    k->p = pmt;
    kid_die (k);
    return false;
  }

  if (k->oaction == kid_sword_normal) {
    if (keep_sword) {
      /* keep_sword_fast = true; */
      kid_keep_sword (k);
      return false;
    }

    if (defense) {
      kid_sword_defense (k);
      return false;
    }

    if (attack) {
      kid_sword_attack (k);
      return false;
    }

    if (walkf) {
      kid_sword_walkf (k);
      return false;
    }

    if (walkb) {
      kid_sword_walkb (k);
      return false;
    }
  }

  k->fo.b = actor_bitmap (k, "KID", "SWORD_NORMAL", 0);
  k->fo.dx = actor_dx (k, "KID", "SWORD_NORMAL", 0);
  k->fo.dy = actor_dy (k, "KID", "SWORD_NORMAL", 0);
  select_actor_xframe (k, "KID", "SWORD", 13);

  k->i++;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide_static_fight (k);

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  if (k->oaction == kid_take_sword)
    update_depressible_floor (k, -2, -27);
  else keep_depressible_floor (k);
}
