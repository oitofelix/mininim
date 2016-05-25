/*
  kid-stop-run.c -- kid stop run module;

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

struct frameset kid_stop_run_frameset[KID_STOP_RUN_FRAMESET_NMEMB];

static void init_kid_stop_run_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_stop_run_00, *kid_stop_run_01,
  *kid_stop_run_02, *kid_stop_run_03;

static void
init_kid_stop_run_frameset (void)
{
  struct frameset frameset[KID_STOP_RUN_FRAMESET_NMEMB] =
    {{kid_stop_run_00,+0,0},{kid_stop_run_01,-1,0},
     {kid_stop_run_02,-21,0},{kid_stop_run_03,-2,0}};

  memcpy (&kid_stop_run_frameset, &frameset,
          KID_STOP_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stop_run (void)
{
  /* bitmaps */
  kid_stop_run_00 = load_bitmap (KID_STOP_RUN_00);
  kid_stop_run_01 = load_bitmap (KID_STOP_RUN_01);
  kid_stop_run_02 = load_bitmap (KID_STOP_RUN_02);
  kid_stop_run_03 = load_bitmap (KID_STOP_RUN_03);

  /* frameset */
  init_kid_stop_run_frameset ();
}

void
unload_kid_stop_run (void)
{
  destroy_bitmap (kid_stop_run_00);
  destroy_bitmap (kid_stop_run_01);
  destroy_bitmap (kid_stop_run_02);
  destroy_bitmap (kid_stop_run_03);
}

void
kid_stop_run (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_stop_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_stop_run) k->i = -1;

  bool turn_run = (k->f.dir == RIGHT) ? k->key.left : k->key.right;
  bool couch = k->key.down;

  if (couch) {
    kid_couch (k);
    return false;
  }

  if (k->i == 3) {
    if (turn_run) kid_turn_run (k);
    else kid_stabilize (k);
    return false;
  }

  select_frame (k, kid_stop_run_frameset, k->i + 1);

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, ptf;

  /* inertia */
  k->inertia = k->cinertia = 0;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)) {
    if (k->i < 2 || k->ci.t != WALL) {
       kid_stabilize_collision (k);
       return false;
      }
    else uncollide (&k->f, &k->fo, &k->fo, -4, false, &k->ci);
  }

  /* fall */
  survey (_tf, pos, &k->f, &nc, &ptf, &np);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  return true;
}

void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -1, -5);
  else if (k->i == 2) update_depressible_floor (k, -2, -7);
  else keep_depressible_floor (k);

  /* sound */
  if (k->i == 1 || k->i == 3) play_sample (step_sample, k->f.c.room);
}

bool
is_kid_stop_run (struct frame *f)
{
  int i;
  for (i = 0; i < KID_STOP_RUN_FRAMESET_NMEMB; i++)
    if (f->b == kid_stop_run_frameset[i].frame) return true;
  return false;
}
