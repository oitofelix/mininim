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

ALLEGRO_BITMAP *kid_sword_normal_08;

void
load_kid_sword_normal (void)
{
  kid_sword_normal_08 = load_bitmap (KID_SWORD_NORMAL_08);
}

void
unload_kid_sword_normal (void)
{
  al_destroy_bitmap (kid_sword_normal_08);
}

void
kid_sword_normal (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_sword_normal;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  bool keep_sword = down_key;
  bool defense = up_key;
  bool attack = shift_key;
  bool walkf = ((kid->f.dir == RIGHT) && right_key)
    || ((kid->f.dir == LEFT) && left_key);
  bool walkb = ((kid->f.dir == RIGHT) && left_key)
    || ((kid->f.dir == LEFT) && right_key);

  if (kid->oaction == kid_sword_normal
      && kid == current_kid) {
    if (keep_sword) {
      /* keep_sword_fast = true; */
      kid_keep_sword (kid);
      return false;
    }

    if (defense) {
      kid_sword_defense (kid);
      return false;
    }

    if (attack) {
      kid_sword_attack (kid);
      return false;
    }

    if (walkf) {
      kid_sword_walkf (kid);
      return false;
    }

    if (walkb) {
      kid_sword_walkb (kid);
      return false;
    }
  }

  kid->fo.b = kid_sword_normal_08;
  kid->fo.dx = kid->fo.dy = +0;
  select_xframe (&kid->xf, sword_frameset, 13);

  if (kid->f.b == kid_take_sword_frameset[3].frame) kid->fo.dx = -4;
  if (kid->f.b == kid_sword_walkf_frameset[1].frame) kid->fo.dx = +5;
  if (kid->f.b == kid_sword_walkb_frameset[1].frame) kid->fo.dx = +2;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* fall */
  survey (_bf, pos, &kid->f, &nc, &pbf, &np);
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  survey (_bb, pos, &kid->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&pbb)) {
    kid->xf.b = NULL;
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->f.b == kid_take_sword_frameset[3].frame)
    update_depressible_floor (kid, -2, -27);
  else keep_depressible_floor (kid);
}
