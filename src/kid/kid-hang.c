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

#include "mininim.h"

struct frameset kid_hang_frameset[KID_HANG_FRAMESET_NMEMB];

static void init_kid_hang_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_hang_00, *kid_hang_01, *kid_hang_02,
  *kid_hang_03, *kid_hang_04, *kid_hang_05, *kid_hang_06,
  *kid_hang_07, *kid_hang_08, *kid_hang_09, *kid_hang_10,
  *kid_hang_11, *kid_hang_12, *kid_hang_13;

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
  kid_hang_13 = load_bitmap (KID_HANG_13);

  /* frameset */
  init_kid_hang_frameset ();
}

void
unload_kid_hang (void)
{
  destroy_bitmap (kid_hang_00);
  destroy_bitmap (kid_hang_01);
  destroy_bitmap (kid_hang_02);
  destroy_bitmap (kid_hang_03);
  destroy_bitmap (kid_hang_04);
  destroy_bitmap (kid_hang_05);
  destroy_bitmap (kid_hang_06);
  destroy_bitmap (kid_hang_07);
  destroy_bitmap (kid_hang_08);
  destroy_bitmap (kid_hang_09);
  destroy_bitmap (kid_hang_10);
  destroy_bitmap (kid_hang_11);
  destroy_bitmap (kid_hang_12);
  destroy_bitmap (kid_hang_13);
}

void
kid_hang (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_hang;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_hang)
    k->i = -1, k->hang_caller = k->oaction, k->misstep = false;

  int dir = (k->f.dir == LEFT) ? 0 : 1;
  place_frame (&k->f, &k->f, kid_hang_13,
               &k->hang_pos, PLACE_WIDTH * dir + 7, -9);

  if (k->i == -1 && k->oaction != kid_unclimb) {
    k->fo.b = kid_hang_13;
    k->fo.dx = +0;
    k->fo.dy = +0;
  } else if (is_hang_pos_free (&k->hang_pos, k->f.dir)) {
    kid_hang_free (k);
    return false;
  } else {
    kid_hang_wall (k);
    return false;
  }

  k->i++;

  return true;
}

static bool
physics_in (struct anim *k)
{
  /* intertia */
  k->inertia = k->cinertia = 0;

  return true;
}

static void
physics_out (struct anim *k)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (k);
  get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}

bool
is_kid_hang (struct frame *f)
{
  int i;
  for (i = 0; i < KID_HANG_FRAMESET_NMEMB; i++)
    if (f->b == kid_hang_frameset[i].frame) return true;

  if (f->b == kid_hang_13) return true;

  return false;
}

bool
is_kid_hang_or_climb (struct frame *f)
{
  return is_kid_hang (f)
    || f->b == kid_climb_00
    || f->b == kid_climb_01
    || f->b == kid_climb_02
    || f->b == kid_climb_03
    || f->b == kid_climb_04
    || f->b == kid_climb_05
    || f->b == kid_climb_06
    || f->b == kid_climb_07
    || f->b == kid_climb_08
    || f->b == kid_climb_09;
}

bool
is_kid_hanging_at_pos (struct frame *f, struct pos *p)
{
  pos_f pf = (f->dir == LEFT) ? posf : pos;

  struct coord bb; struct pos pbb, npbb, pr;
  survey (_bb, pf, f, &bb, &pbb, &npbb);

  int dir = (f->dir == LEFT) ? -1 : +1;
  return (is_kid_hang_or_climb (f)
          && peq (prel (&pbb, &pr, -1, dir), p));
}
