/*
  kid-sword-walkb.c -- kid sword walk backward module;

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

struct frameset kid_sword_walkb_frameset[KID_SWORD_WALKB_FRAMESET_NMEMB];

static void init_kid_sword_walkb_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_walkb_00, *kid_sword_walkb_01;

static void
init_kid_sword_walkb_frameset (void)
{
  struct frameset frameset[KID_SWORD_WALKB_FRAMESET_NMEMB] =
    {{kid_sword_walkb_00,-1,0},{kid_sword_walkb_01,+11,0}};

  memcpy (&kid_sword_walkb_frameset, &frameset,
          KID_SWORD_WALKB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_walkb (void)
{
  /* bitmaps */
  kid_sword_walkb_00 = load_bitmap (KID_SWORD_WALKB_00);
  kid_sword_walkb_01 = load_bitmap (KID_SWORD_WALKB_01);

  /* frameset */
  init_kid_sword_walkb_frameset ();
}

void
unload_kid_sword_walkb (void)
{
  destroy_bitmap (kid_sword_walkb_00);
  destroy_bitmap (kid_sword_walkb_01);
}

void
kid_sword_walkb (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_walkb;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_sword_walkb) k->i = -1;

  if (k->i == 1) {
    kid_sword_normal (k);
    return false;
  }

  if (k->i == -1) k->j = 10;
  if (k->i == 0) k->j = 17;

  if (k->f.b == kid_sword_attack_frameset[5].frame) k->i = 0;

  select_frame (k, kid_sword_walkb_frameset, k->i + 1);
  select_xframe (&k->xf, sword_frameset, k->j);

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbf, pmbo, pbb;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +0, true, &k->ci);

  /* fall */
  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  survey (_bb, pos, &k->f, &nc, &pbb, &np);
  if ((is_strictly_traversable (&pbf)
       || is_strictly_traversable (&pmbo)
       || is_strictly_traversable (&pbb))) {
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
  if (k->i == 1) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}
