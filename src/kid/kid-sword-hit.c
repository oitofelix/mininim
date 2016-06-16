/*
  kid-sword-hit.c -- kid sword walk forward module;

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

struct frameset kid_sword_hit_frameset[KID_SWORD_HIT_FRAMESET_NMEMB];

static void init_kid_sword_hit_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_sword_hit_00, *kid_sword_hit_01,
  *kid_sword_hit_02, *kid_sword_hit_03, *kid_sword_hit_04;

static void
init_kid_sword_hit_frameset (void)
{
  struct frameset frameset[KID_SWORD_HIT_FRAMESET_NMEMB] =
    {{kid_sword_hit_00,+0,0},{kid_sword_hit_01,+0,0},
     {kid_sword_hit_02,+4,0},{kid_sword_hit_03,+8,0},
     {kid_sword_hit_04,+8,0}};

  memcpy (&kid_sword_hit_frameset, &frameset,
          KID_SWORD_HIT_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_sword_hit (void)
{
  /* bitmaps */
  kid_sword_hit_00 = load_bitmap (KID_SWORD_HIT_00);
  kid_sword_hit_01 = load_bitmap (KID_SWORD_HIT_01);
  kid_sword_hit_02 = load_bitmap (KID_SWORD_HIT_02);
  kid_sword_hit_03 = load_bitmap (KID_SWORD_HIT_03);
  kid_sword_hit_04 = load_bitmap (KID_SWORD_HIT_04);

  /* frameset */
  init_kid_sword_hit_frameset ();
}

void
unload_kid_sword_hit (void)
{
  destroy_bitmap (kid_sword_hit_00);
  destroy_bitmap (kid_sword_hit_01);
  destroy_bitmap (kid_sword_hit_02);
  destroy_bitmap (kid_sword_hit_03);
  destroy_bitmap (kid_sword_hit_04);
}

void
kid_sword_hit (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_sword_hit;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_sword_hit) {
    k->i = -1;
    k->j = 0;
  }

  if (k->i == 4) {
    kid_sword_normal (k);
    return false;
  }

  select_frame (k, kid_sword_hit_frameset, k->i + 1);

  if (k->i == 0) k->j = 28;
  if (k->i == 1) k->j = 32;
  if (k->i == 2) k->j = 29;
  if (k->i == 3) k->j = 7;
  if (k->i == 4) k->j = 17;

  select_xframe (&k->xf, sword_frameset, k->j);

  if (k->i == 0) k->xf.dx = -12, k->xf.dy = +2;
  if (k->i == 1) k->xf.dy += -2;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pbf, pmbo, pbb;

  /* collision */
  uncollide (&k->f, &k->fo, &k->fo, +0, true, &k->ci);

  /* fall */
  survey (_bf, pos, &k->f, NULL, &pbf, NULL);
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  survey (_bb, pos, &k->f, NULL, &pbb, NULL);
  if (is_strictly_traversable (&pbf)
      || is_strictly_traversable (&pmbo)
      /* || is_strictly_traversable (&pbb) */
      ) {
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
  if (k->i == 3) update_depressible_floor (k, -4, -33);
  else if (k->i == 4) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}
