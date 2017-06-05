/*
  kid-normal.c -- kid normal module;

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
kid_normal (struct actor *k)
{
  k->next_action = NULL;

  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_normal;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (k->ignore_danger == 2) k->ignore_danger = 0;

  if (! flow (k)) {
    k->misstep = false;
    k->ignore_danger = 0;
    return;
  }
  if (! cutscene && ! physics_in (k)) {
    k->misstep = false;
    k->ignore_danger = 0;
    return;
  }
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);

  if (k->next_action == kid_walk && k->ignore_danger == 1)
    k->ignore_danger = 2;
}

static bool
flow (struct actor *k)
{
  struct pos pmt;

  k->collision = false;
  k->hit_by_loose_floor = false;

  bool turn =
    (((k->f.dir == RIGHT) && k->key.left)
     || ((k->f.dir == LEFT) && k->key.right))
    && ! k->key.ctrl;
  bool walk =
    (((k->f.dir == RIGHT) && k->key.right && k->key.shift)
     || ((k->f.dir == LEFT) && k->key.left && k->key.shift))
    && ! k->key.ctrl;
  bool run =
    (((k->f.dir == RIGHT) && k->key.right)
     || ((k->f.dir == LEFT) && k->key.left))
    && ! walk && ! k->key.ctrl;
  bool jump =
    (((k->f.dir == RIGHT) && k->key.right && k->key.up)
     || ((k->f.dir == LEFT) && k->key.left && k->key.up))
    && ! k->key.ctrl;
  bool crouch = k->key.down;
  bool vjump = k->key.up && ! k->key.ctrl && ! k->key.alt
    && anim_cycle - k->selection_cycle > SEC2CYC (2);
  bool take_sword = k->key.enter && k->has_sword;

  /* acquire item */
  invalid_pos (&k->item_pos);
  if (k->key.shift && ! walk) {
    struct pos pbf; survey (_bf, pos, &k->f, NULL, &pbf, NULL);
    struct pos pbf2; survey (_bf, posf, &k->f, NULL, &pbf2, NULL);
    if (is_potion (&pbf) || is_sword (&pbf))
      k->item_pos = pbf;
    else if (is_potion (&pbf2) || is_sword (&pbf2))
      k->item_pos = pbf2;
  }

  survey (_mt, pos, &k->f, NULL, &pmt, NULL);
  bool stairs = k->key.up && ! k->key.left && ! k->key.right
    && k->shadow_of == - 1 && is_in_front_open_level_door (&k->f, &k->p);

  if (k->oaction == kid_normal
      && k->current_hp <= 0) {
    survey (_mt, pos, &k->f, NULL, &pmt, NULL);
    k->p = pmt;
    kid_die (k);
    return false;
  }

  if (k->oaction == kid_normal) {
    if (stairs) {
      kid_stairs (k);
      return false;
    }

    if (crouch) {
      kid_crouch (k);
      return false;
    }

    if (jump) {
      kid_jump (k);
      return false;
    }

    if (turn) {
      kid_turn (k);
      return false;
    }

    if (vjump) {
      struct pos p;
      if (is_in_front_level_door (&k->f, &p)
          && ! is_in_front_open_level_door (&k->f, NULL)) {
        struct level_door *d =
          level_door_at_pos (&p);
        if (d->action != OPEN_LEVEL_DOOR) {
          kid_vjump (k);
          return false;
        }
      } else {
        kid_vjump (k);
        return false;
      }
    }
    if (walk) {
      if (k->misstep) k->next_action = kid_misstep;
      else k->next_action = kid_walk;
    }

    if (run) {
      if (dist_collision (&k->f, _bf, -4, -4, &k->ci) < 29)
        kid_walk (k);
      else kid_start_run (k);
      return false;
    }

    if (is_valid_pos (&k->item_pos)) {
      if (is_potion (&k->item_pos)) {
        int dx = (k->f.dir == LEFT) ? PLACE_WIDTH + 3 : +9;
        int dy = +27;
        place_actor (k, &k->item_pos, dx, dy, "KID", "CROUCH", 0);
      }
      kid_crouch (k);
      return false;
    }

    if (take_sword) {
      kid_take_sword (k);
      return false;
    }
  }

  k->fo.b = actor_bitmap (k, "KID", "NORMAL", 0);
  k->fo.dx = actor_dx (k, "KID", "NORMAL", 0);
  k->fo.dy = actor_dy (k, "KID", "NORMAL", 0);

  k->xf.b = NULL;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = k->cinertia = 0;

  /* collision */
  uncollide_static_kid_normal (k);

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)
      && is_falling (&k->f, _bb, +0, +0)) {
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
  keep_depressible_floor (k);
}
