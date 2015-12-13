/*
  kid-hang.c -- kid hang module;

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

struct frameset kid_hang_frameset[KID_HANG_FRAMESET_NMEMB];

static void init_kid_hang_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_hang_00, *kid_hang_01, *kid_hang_02,
  *kid_hang_03, *kid_hang_04, *kid_hang_05, *kid_hang_06,
  *kid_hang_07, *kid_hang_08, *kid_hang_09, *kid_hang_10,
  *kid_hang_11, *kid_hang_12, *kid_hang_14;

static void
init_kid_hang_frameset (void)
{
  struct frameset frameset[KID_HANG_FRAMESET_NMEMB] =
    {{kid_hang_00,+0,+0},{kid_hang_01,+0,+2},{kid_hang_02,+4,+0},
     {kid_hang_03,+3,+2},{kid_hang_04,+3,+0},{kid_hang_05,+1,+0},
     {kid_hang_06,+1,-1},{kid_hang_07,+2,+0},{kid_hang_08,+0,-3},
     {kid_hang_09,+0,+0},{kid_hang_10,+1,-1},{kid_hang_11,+0,+0},
     {kid_hang_12,-3,+0}};

  memcpy (&kid_hang_frameset, &frameset,
          KID_HANG_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_hang (void)
{
  /* bitmaps */
  kid_hang_00 = load_bitmap (KID_HANG_00);
  kid_hang_01 = load_bitmap (KID_HANG_01);
  kid_hang_02 = load_bitmap (KID_HANG_02);
  kid_hang_03 = load_bitmap (KID_HANG_03);
  kid_hang_04 = load_bitmap (KID_HANG_04);
  kid_hang_05 = load_bitmap (KID_HANG_05);
  kid_hang_06 = load_bitmap (KID_HANG_06);
  kid_hang_07 = load_bitmap (KID_HANG_07);
  kid_hang_08 = load_bitmap (KID_HANG_08);
  kid_hang_09 = load_bitmap (KID_HANG_09);
  kid_hang_10 = load_bitmap (KID_HANG_10);
  kid_hang_11 = load_bitmap (KID_HANG_11);
  kid_hang_12 = load_bitmap (KID_HANG_12);
  kid_hang_14 = load_bitmap (KID_HANG_14);

  /* frameset */
  init_kid_hang_frameset ();
}

void
unload_kid_hang (void)
{
  al_destroy_bitmap (kid_hang_00);
  al_destroy_bitmap (kid_hang_01);
  al_destroy_bitmap (kid_hang_02);
  al_destroy_bitmap (kid_hang_03);
  al_destroy_bitmap (kid_hang_04);
  al_destroy_bitmap (kid_hang_05);
  al_destroy_bitmap (kid_hang_06);
  al_destroy_bitmap (kid_hang_07);
  al_destroy_bitmap (kid_hang_08);
  al_destroy_bitmap (kid_hang_09);
  al_destroy_bitmap (kid_hang_10);
  al_destroy_bitmap (kid_hang_11);
  al_destroy_bitmap (kid_hang_12);
  al_destroy_bitmap (kid_hang_14);
}

void
kid_hang (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_hang;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_hang)
    kid->i = -1, kid->hang_caller = kid->oaction, kid->misstep = false;

  int dir = (kid->f.dir == LEFT) ? 0 : 1;
  place_frame (&kid->f, &kid->f, kid_hang_14,
               &kid->hang_pos, PLACE_WIDTH * dir + 7, -9);

  if (kid->i == -1 && kid->oaction != kid_unclimb) {
    kid->fo.b = kid_hang_14;
    kid->fo.dx = +0;
    kid->fo.dy = +0;
  } else if (is_hang_pos_free (&kid->hang_pos, kid->f.dir)) {
    kid_hang_free (kid);
    return false;
  } else {
    kid_hang_wall (kid);
    return false;
  }

  kid->i++;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  /* intertia */
  kid->inertia = 0;

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (kid);
  get_hanged_pos (&kid->hang_pos, kid->f.dir, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}

bool
is_kid_hang_or_climb (struct frame *f)
{
  return f->b == kid_hang_00
    || f->b == kid_hang_01
    || f->b == kid_hang_02
    || f->b == kid_hang_03
    || f->b == kid_hang_04
    || f->b == kid_hang_05
    || f->b == kid_hang_06
    || f->b == kid_hang_07
    || f->b == kid_hang_08
    || f->b == kid_hang_09
    || f->b == kid_hang_10
    || f->b == kid_hang_11
    || f->b == kid_hang_12
    || f->b == kid_hang_14

    || f->b == kid_climb_01
    || f->b == kid_climb_02
    || f->b == kid_climb_03
    || f->b == kid_climb_04
    || f->b == kid_climb_05
    || f->b == kid_climb_06
    || f->b == kid_climb_07
    || f->b == kid_climb_08
    || f->b == kid_climb_09
    || f->b == kid_climb_10;
}

bool
is_kid_hanging_at_pos (struct frame *f, struct pos *p)
{
  struct coord bb; struct pos pbb, npbb, pr;
  survey (_bb, pos, f, &bb, &pbb, &npbb);

  int dir = (f->dir == LEFT) ? -1 : +1;
  return (is_kid_hang_or_climb (f)
          && peq (prel (&pbb, &pr, -1, dir), p));
}
