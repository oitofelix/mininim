/*
  kid-sword-normal.c -- kid sword normal module;

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

static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_normal_00;

void
load_kid_sword_normal (void)
{
  kid_sword_normal_00 = load_bitmap (KID_SWORD_NORMAL_00);
}

void
unload_kid_sword_normal (void)
{
  destroy_bitmap (kid_sword_normal_00);
}

void
kid_sword_normal (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_normal;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct coord nc;
  struct pos np, pmt;

  bool keep_sword = k->key.down;
  bool defense = k->key.up && ! k->key.shift
    && ! k->key.left && ! k->key.right;
  bool attack = k->key.shift && ! k->key.up
    && ! k->key.left && ! k->key.right;
  bool walkf = ((k->f.dir == RIGHT) && k->key.right)
    || ((k->f.dir == LEFT) && k->key.left);
  bool walkb = ((k->f.dir == RIGHT) && k->key.left)
    || ((k->f.dir == LEFT) && k->key.right);

  k->keep_sword_fast = k->enemy_id != -1;

  if (k->oaction != kid_sword_normal) k->i = -1;

  if (k->oaction == kid_sword_normal
      && k->current_lives <= 0) {
    survey (_mt, pos, &k->f, &nc, &pmt, &np);
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

  k->fo.b = kid_sword_normal_00;
  k->fo.dx = k->fo.dy = +0;
  select_xframe (&k->xf, sword_frameset, 13);

  if (k->f.b == kid_take_sword_frameset[3].frame) k->fo.dx = -4;
  if (k->f.b == kid_sword_walkf_frameset[1].frame) k->fo.dx = +5;
  if (k->f.b == kid_sword_walkb_frameset[1].frame) k->fo.dx = +2;

  k->i++;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +0, true, &k->ci);
  if (fight_door_split_collision (k)) return false;

  /* fall */
  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->f.b == kid_take_sword_frameset[3].frame)
    update_depressible_floor (k, -2, -27);
  else keep_depressible_floor (k);
}
