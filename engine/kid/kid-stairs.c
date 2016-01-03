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

#include "prince.h"
#include "kernel/video.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/physics.h"
#include "engine/door.h"
#include "engine/potion.h"
#include "engine/sword.h"
#include "engine/loose-floor.h"
#include "engine/level.h"
#include "kid.h"

struct frameset kid_stairs_frameset[KID_STAIRS_FRAMESET_NMEMB];

static void init_kid_stairs_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_stairs_01, *kid_stairs_02, *kid_stairs_03,
  *kid_stairs_04, *kid_stairs_05, *kid_stairs_06, *kid_stairs_07,
  *kid_stairs_08, *kid_stairs_09, *kid_stairs_10, *kid_stairs_11,
  *kid_stairs_12;

static void
init_kid_stairs_frameset (void)
{
  struct frameset frameset[KID_STAIRS_FRAMESET_NMEMB] =
    {{kid_stairs_01,+0,+0},{kid_stairs_02,-1,+0},{kid_stairs_03,+0,+0},
     {kid_stairs_04,-2,+0},{kid_stairs_05,+10,-3},{kid_stairs_06,+8,-2},
     {kid_stairs_07,+4,-3},{kid_stairs_08,+7,-8},{kid_stairs_09,+4,-1},
     {kid_stairs_10,+5,-4},{kid_stairs_11,+3,-6},{kid_stairs_12,+4,+0}};

  memcpy (&kid_stairs_frameset, &frameset,
          KID_STAIRS_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stairs (void)
{
  /* bitmaps */
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
  kid_stairs_12 = load_bitmap (KID_STAIRS_12);

  /* frameset */
  init_kid_stairs_frameset ();
}

void
unload_kid_stairs (void)
{
  al_destroy_bitmap (kid_stairs_01);
  al_destroy_bitmap (kid_stairs_02);
  al_destroy_bitmap (kid_stairs_03);
  al_destroy_bitmap (kid_stairs_04);
  al_destroy_bitmap (kid_stairs_05);
  al_destroy_bitmap (kid_stairs_06);
  al_destroy_bitmap (kid_stairs_07);
  al_destroy_bitmap (kid_stairs_08);
  al_destroy_bitmap (kid_stairs_09);
  al_destroy_bitmap (kid_stairs_10);
  al_destroy_bitmap (kid_stairs_11);
  al_destroy_bitmap (kid_stairs_12);
}

void
kid_stairs (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_stairs;
  kid->f.dir = RIGHT;
  kid->f.flip = 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame_inv = true;
  next_frame (&kid->f, &kid->f, &kid->fo);
  next_frame_inv = false;
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_stairs) {
    place_frame (&kid->f, &kid->f, kid_stairs_frameset[0].frame,
                 &kid->p, +3, +16);
    kid->i = kid->j = -1;
  }

  if (kid->i == 11) level.end ();

  if (kid->i >= 11 && kid->j < 17) {
    kid->invisible = true;
    kid->j++;
  } else if (kid->i < 11) kid->j = kid->i + 1;

  int i = kid->i <= 10 ? kid->i + 1: 11;

  select_frame (kid, kid_stairs_frameset, i);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i > 4) clear_depressible_floor (kid);
  else keep_depressible_floor (kid);

  /* sound */
  if (kid->j % 4 == 0) play_sample (step_sample);
}

bool
is_kid_stairs (struct frame *f)
{
  int i;
  for (i = 0; i < KID_STAIRS_FRAMESET_NMEMB; i++)
    if (f->b == kid_stairs_frameset[i].frame) return true;

  return false;
}
