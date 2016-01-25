/*
  kid-misstep.c -- kid misstep module;

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

struct frameset kid_misstep_frameset[KID_MISSTEP_FRAMESET_NMEMB];

static void init_kid_misstep_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

static void
init_kid_misstep_frameset (void)
{
  struct frameset frameset[KID_MISSTEP_FRAMESET_NMEMB] =
    {{kid_walk_00,-1,0},{kid_walk_01,-1,0},{kid_walk_02,+0,0},
     {kid_walk_03,-8,0},{kid_walk_04,-7,0},{kid_walk_05,-4,0},
     {kid_jump_13,+8,0},{kid_couch_09,+8,0},{kid_couch_10,+3,0},
     {kid_couch_11,+0,0},{kid_couch_12,+4,0}};

  memcpy (&kid_misstep_frameset, &frameset,
          KID_MISSTEP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_misstep (void)
{
  /* frameset */
  init_kid_misstep_frameset ();
}

void
unload_kid_misstep (void)
{
  /* for symmetry */
}

void
kid_misstep (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_misstep;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_misstep) k->i = -1, k->misstep = true;

  if (k->i == 10) {
    place_frame (&k->f, &k->f, kid_normal_00, &k->p,
                 (k->f.dir == LEFT) ? +11 : PLACE_WIDTH + 7, 15);
    kid_normal (k);
    return false;
  }

  select_frame (k, kid_misstep_frameset, k->i + 1);

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pmba;

  /* fall */
  survey (_mba, pos, &k->f, &nc, &pmba, &np);
  if (is_strictly_traversable (&pmba)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  struct coord nc; struct pos pmbo, np;

  /* depressible floors */
  keep_depressible_floor (k);

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  if (k->i == 8) shake_loose_floor_row (&pmbo);

  /* sound */
  if (k->i == 7) play_sample (step_sample, k->f.c.room);
}
