/*
  kid-take-sword.c -- kid take sword module;

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

struct frameset kid_take_sword_frameset[KID_TAKE_SWORD_FRAMESET_NMEMB];

static void init_kid_take_sword_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_take_sword_00, *kid_take_sword_01,
  *kid_take_sword_02, *kid_take_sword_03;

static void
init_kid_take_sword_frameset (void)
{
  struct frameset frameset[KID_TAKE_SWORD_FRAMESET_NMEMB] =
    {{kid_take_sword_00,-6,+0},{kid_take_sword_01,+0,0},
     {kid_take_sword_02,-4,0},{kid_take_sword_03,-6,0}};

  memcpy (&kid_take_sword_frameset, &frameset,
          KID_TAKE_SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_take_sword (void)
{
  /* bitmaps */
  kid_take_sword_00 = load_bitmap (KID_TAKE_SWORD_00);
  kid_take_sword_01 = load_bitmap (KID_TAKE_SWORD_01);
  kid_take_sword_02 = load_bitmap (KID_TAKE_SWORD_02);
  kid_take_sword_03 = load_bitmap (KID_TAKE_SWORD_03);

  /* frameset */
  init_kid_take_sword_frameset ();
}

void
unload_kid_take_sword (void)
{
  destroy_bitmap (kid_take_sword_00);
  destroy_bitmap (kid_take_sword_01);
  destroy_bitmap (kid_take_sword_02);
  destroy_bitmap (kid_take_sword_03);
}

void
kid_take_sword (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_take_sword;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_take_sword) k->i = -1;

  if (k->i == 3) {
    kid_sword_normal (k);
    return false;
  }

  select_frame (k, kid_take_sword_frameset, k->i + 1);

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord tf; struct pos np, ptf;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +14, false, &k->ci);

  /* fall */
  survey (_tf, pos, &k->f, &tf, &ptf, &np);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* sound */
  if (k->i == 0) play_sample (take_sword_sample, k->f.c.room);

  /* depressible floors */
  if (k->i == 3) update_depressible_floor (k, -4, -23);
  else keep_depressible_floor (k);
}
