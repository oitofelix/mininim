/*
  kid-stairs.c -- kid stairs module;

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

struct frameset kid_stairs_frameset[KID_STAIRS_FRAMESET_NMEMB];

static void init_kid_stairs_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_stairs_00, *kid_stairs_01, *kid_stairs_02,
  *kid_stairs_03, *kid_stairs_04, *kid_stairs_05, *kid_stairs_06,
  *kid_stairs_07, *kid_stairs_08, *kid_stairs_09, *kid_stairs_10,
  *kid_stairs_11;

static void
init_kid_stairs_frameset (void)
{
  struct frameset frameset[KID_STAIRS_FRAMESET_NMEMB] =
    {{kid_stairs_00,+0,+0},{kid_stairs_01,-1,+0},{kid_stairs_02,+0,+0},
     {kid_stairs_03,-2,+0},{kid_stairs_04,+10,-3},{kid_stairs_05,+8,-2},
     {kid_stairs_06,+4,-3},{kid_stairs_07,+7,-8},{kid_stairs_08,+4,-1},
     {kid_stairs_09,+5,-4},{kid_stairs_10,+3,-6},{kid_stairs_11,+4,+0}};

  memcpy (&kid_stairs_frameset, &frameset,
          KID_STAIRS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stairs (void)
{
  /* bitmaps */
  kid_stairs_00 = load_bitmap (KID_STAIRS_00);
  kid_stairs_01 = load_bitmap (KID_STAIRS_01);
  kid_stairs_02 = load_bitmap (KID_STAIRS_02);
  kid_stairs_03 = load_bitmap (KID_STAIRS_03);
  kid_stairs_04 = load_bitmap (KID_STAIRS_04);
  kid_stairs_05 = load_bitmap (KID_STAIRS_05);
  kid_stairs_06 = load_bitmap (KID_STAIRS_06);
  kid_stairs_07 = load_bitmap (KID_STAIRS_07);
  kid_stairs_08 = load_bitmap (KID_STAIRS_08);
  kid_stairs_09 = load_bitmap (KID_STAIRS_09);
  kid_stairs_10 = load_bitmap (KID_STAIRS_10);
  kid_stairs_11 = load_bitmap (KID_STAIRS_11);

  /* frameset */
  init_kid_stairs_frameset ();
}

void
unload_kid_stairs (void)
{
  destroy_bitmap (kid_stairs_00);
  destroy_bitmap (kid_stairs_01);
  destroy_bitmap (kid_stairs_02);
  destroy_bitmap (kid_stairs_03);
  destroy_bitmap (kid_stairs_04);
  destroy_bitmap (kid_stairs_05);
  destroy_bitmap (kid_stairs_06);
  destroy_bitmap (kid_stairs_07);
  destroy_bitmap (kid_stairs_08);
  destroy_bitmap (kid_stairs_09);
  destroy_bitmap (kid_stairs_10);
  destroy_bitmap (kid_stairs_11);
}

void
kid_stairs (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_stairs;
  k->f.dir = RIGHT;
  k->f.flip = 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame_inv = true;
  next_frame (&k->f, &k->f, &k->fo);
  next_frame_inv = false;
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_stairs) {
    place_frame (&k->f, &k->f, kid_stairs_frameset[0].frame,
                 &k->p, +3, +16);
    k->i = k->j = -1;
  }

  if (k->i >= 9) if (level.end) level.end (&k->p);

  if (k->i == 11 && k->j < 17) {
    k->invisible = true;
    k->j++;
  } else if (k->i < 11) k->j = k->i + 1;

  int i = k->i <= 10 ? k->i + 1: 11;

  select_frame (k, kid_stairs_frameset, i);

  if (k->j > 11) k->fo.dx = k->fo.dy = 0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->i > 4) clear_depressible_floor (k);
  else keep_depressible_floor (k);

  /* sound */
  if (k->j % 4 == 0) play_sample (step_sample, k->f.c.room);
}

bool
is_kid_stairs (struct frame *f)
{
  int i;
  for (i = 0; i < KID_STAIRS_FRAMESET_NMEMB; i++)
    if (f->b == kid_stairs_frameset[i].frame) return true;

  return false;
}
