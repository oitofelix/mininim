/*
  kid-normal.c -- kid normal module;

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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/level.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "kid.h"

static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_normal_00;

void
load_kid_normal (void)
{
  kid_normal_00 = load_bitmap (KID_NORMAL_00);
}

void
unload_kid_normal (void)
{
  al_destroy_bitmap (kid_normal_00);
}

void
kid_normal (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_normal;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct coord nc;
  struct pos np, pbf, pmt;
  survey (_bf, pos, &k->f, &nc, &pbf, &np);

  bool turn = ((k->f.dir == RIGHT) && left_key)
    || ((k->f.dir == LEFT) && right_key);
  bool walk = ((k->f.dir == RIGHT) && right_key && shift_key)
    || ((k->f.dir == LEFT) && left_key && shift_key);
  bool run = (((k->f.dir == RIGHT) && right_key)
              || ((k->f.dir == LEFT) && left_key)) && ! walk;
  bool jump = ((k->f.dir == RIGHT) && right_key && up_key)
    || ((k->f.dir == LEFT) && left_key && up_key);
  bool couch = down_key;
  bool vjump = up_key;
  bool drink = is_potion (&pbf) && shift_key;
  bool raise_sword = is_sword (&pbf) && shift_key;
  bool take_sword = ctrl_key;

  if (k->oaction == kid_normal
      && k->current_lives <= 0) {
    survey (_mt, pos, &k->f, &nc, &pmt, &np);
    k->death_pos = pmt;
    kid_die (k);
    return false;
  }

  if (k->oaction == kid_normal
      && k == current_kid) {
    if (couch) {
      kid_couch (k);
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
      kid_vjump (k);
      return false;
    }
    if (walk) {
      kid_walk (k);
      return false;
    }

    if (run) {
      if (dist_collision (&k->f, false, &kid->ci) + 4 < 29)
        kid_walk (k);
      else kid_start_run (k);
      return false;
    }

    if (drink) {
      k->item_pos = pbf;
      place_frame (&k->f, &k->f, kid_couch_frameset[0].frame,
                   &k->item_pos, (k->f.dir == LEFT)
                   ? PLACE_WIDTH + 3 : +9, +27);
      kid_couch (k);
      return false;
    }

    if (raise_sword) {
      k->item_pos = pbf;
      kid_couch (k);
      return false;
    }

    if (take_sword) {
      kid_take_sword (k);
      return false;
    }
  }

  k->fo.b = kid_normal_00;
  k->fo.dx = k->fo.dy = +0;

  if (k->f.b == kid_stabilize_frameset[3].frame) k->fo.dx = +2;
  if (k->f.b == kid_walk_frameset[11].frame) k->fo.dx = -1;
  if (k->f.b == kid_jump_frameset[17].frame) k->fo.dx = -2;
  if (k->f.b == kid_couch_frameset[12].frame) k->fo.dx = -2;
  if (k->f.b == kid_vjump_frameset[17].frame) k->fo.dx = +2;
  if (k->f.b == kid_drink_frameset[7].frame) k->fo.dx = +0;
  if (k->f.b == kid_keep_sword_frameset[9].frame) k->fo.dx = +2;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pmbo, pbb;

  /* inertia */
  k->inertia = 0;

  /* fall */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pmbo)
      && is_strictly_traversable (&pbb)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  keep_depressible_floor (k);
}
