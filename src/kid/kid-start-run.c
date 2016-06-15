/*
  kid-start-run.c -- kid start run module;

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

struct frameset kid_start_run_frameset[KID_START_RUN_FRAMESET_NMEMB];

static void init_kid_start_run_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_start_run_00, *kid_start_run_01, *kid_start_run_02,
  *kid_start_run_03, *kid_start_run_04, *kid_start_run_05;

static void
init_kid_start_run_frameset (void)
{
  struct frameset frameset[KID_START_RUN_FRAMESET_NMEMB] =
    {{kid_start_run_00,-1,0},{kid_start_run_01,-2,0},
     {kid_start_run_02,-5,0},{kid_start_run_03,-1,0},
     {kid_start_run_04,-7,0},{kid_start_run_05,-6,0}};

  memcpy (&kid_start_run_frameset, &frameset,
          KID_START_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_start_run (void)
{
  /* bitmaps */
  kid_start_run_00 = load_bitmap (KID_START_RUN_00);
  kid_start_run_01 = load_bitmap (KID_START_RUN_01);
  kid_start_run_02 = load_bitmap (KID_START_RUN_02);
  kid_start_run_03 = load_bitmap (KID_START_RUN_03);
  kid_start_run_04 = load_bitmap (KID_START_RUN_04);
  kid_start_run_05 = load_bitmap (KID_START_RUN_05);

  /* frameset */
  init_kid_start_run_frameset ();
}

void
unload_kid_start_run (void)
{
  destroy_bitmap (kid_start_run_00);
  destroy_bitmap (kid_start_run_01);
  destroy_bitmap (kid_start_run_02);
  destroy_bitmap (kid_start_run_03);
  destroy_bitmap (kid_start_run_04);
  destroy_bitmap (kid_start_run_05);
}

void
kid_start_run (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_start_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_start_run) k->i = -1, k->misstep = false;

  bool run = (k->f.dir == RIGHT) ? k->key.right : k->key.left;
  bool turn_run = (k->f.dir == RIGHT) ? k->key.left : k->key.right;
  bool couch = k->key.down;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);

  if (couch) {
    kid_couch (k);
    return false;
  }

  if (jump && k->i < 3) {
    kid_jump (k);
    return false;
  }

  if ((k->i == 3 || k->i == 4) && turn_run) {
    kid_stop_run (k);
    return false;
  }

  if (k->i == 5) {
    if (run) kid_run (k);
    else kid_stop_run (k);
    return false;
  }

  select_frame (k, kid_start_run_frameset, k->i + 1);

  if (k->f.b == kid_turn_frameset[3].frame) k->fo.dx = +0;
  if (k->f.b == kid_stabilize_frameset[0].frame) k->fo.dx = +2;
  if (k->f.b == kid_stabilize_frameset[1].frame) k->fo.dx = +6;
  if (k->f.b == kid_stabilize_frameset[2].frame) k->fo.dx = +4;
  if (k->f.b == kid_stabilize_frameset[3].frame) k->fo.dx = +0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos pmbo, ptf;

  /* inertia */
  k->inertia = 0;
  k->cinertia = 2;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)) {
    kid_stabilize_collision (k);
    return false;
  }

  /* fall */
  survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);
  if (is_strictly_traversable (&pmbo)
      || is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->i == 5) update_depressible_floor (k, -16, -19);
  else keep_depressible_floor (k);
}

bool
is_kid_start_run (struct frame *f)
{
  int i;
  for (i = 0; i < KID_START_RUN_FRAMESET_NMEMB; i++)
    if (f->b == kid_start_run_frameset[i].frame) return true;
  return false;
}
