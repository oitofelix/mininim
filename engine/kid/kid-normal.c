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
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "kid.h"

static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

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
kid_normal (void)
{
  kid.oaction = kid.action;
  kid.action = kid_normal;
  kid.f.flip = (kid.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  struct coord nc;
  struct pos np, pbf;
  survey (_bf, pos, &kid->f, &nc, &pbf, &np);

  int dc = dist_collision (&kid->f, _tf, pos, -4, false);

  bool turn = ((kid->f.dir == RIGHT) && left_key)
    || ((kid->f.dir == LEFT) && right_key);
  bool walk = ((kid->f.dir == RIGHT) && right_key && shift_key)
    || ((kid->f.dir == LEFT) && left_key && shift_key);
  bool run = (((kid->f.dir == RIGHT) && right_key)
              || ((kid->f.dir == LEFT) && left_key)) && ! walk;
  bool jump = ((kid->f.dir == RIGHT) && right_key && up_key)
    || ((kid->f.dir == LEFT) && left_key && up_key);
  bool couch = down_key;
  bool vjump = up_key;
  bool drink = is_potion (&pbf) && shift_key && dc > 16;
  bool raise_sword = is_sword (&pbf) && shift_key && dc > 16;
  bool take_sword = ctrl_key;

  if (kid->oaction == kid_normal) {
    if (couch) {
      kid_couch ();
      return false;
    }

    if (jump) {
      kid_jump ();
      return false;
    }

    if (turn) {
      kid_turn ();
      return false;
    }

    if (vjump) {
      kid_vjump ();
      return false;
    }
    if (walk) {
      kid_walk ();
      return false;
    }

    if (run) {
      if (dist_collision (&kid->f, _tf, pos, -4, false) < 29)
        kid_walk ();
      else kid_start_run ();
      return false;
    }

    if (drink) {
      item_pos = pbf;
      /* keep this value this way, or the kid might fall if on edge */
      int d = (kid->f.dir == LEFT) ? +10 : +12;
      to_next_place_edge (&kid->f, &kid->f, kid->fo.b, _bf, pos, 0, true, d);
      kid_couch ();
      return false;
    }

    if (raise_sword) {
      item_pos = pbf;
      kid_couch ();
      return false;
    }

    if (take_sword) {
      kid_take_sword ();
      return false;
    }
  }

  kid->fo.b = kid_normal_00;
  kid->fo.dx = kid->fo.dy = +0;

  if (kid->f.b == kid_stabilize_frameset[3].frame) kid->fo.dx = +2;
  if (kid->f.b == kid_walk_frameset[11].frame) kid->fo.dx = -1;
  if (kid->f.b == kid_jump_frameset[17].frame) kid->fo.dx = -2;
  if (kid->f.b == kid_couch_frameset[12].frame) kid->fo.dx = -2;
  if (kid->f.b == kid_vjump_frameset[17].frame) kid->fo.dx = +2;
  if (kid->f.b == kid_drink_frameset[7].frame) kid->fo.dx = +4;
  if (kid->f.b == kid_keep_sword_frameset[9].frame) kid->fo.dx = +2;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cmbo, cbb;

  /* inertia */
  inertia = 0;

  /* fall */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  if (cmbo == NO_FLOOR && cbb == NO_FLOOR) {
    kid_fall ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  keep_depressible_floor (kid);
}
