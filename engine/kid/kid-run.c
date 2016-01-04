/*
  kid-run.c -- kid run module;

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

struct frameset kid_run_frameset[KID_RUN_FRAMESET_NMEMB];

static void init_kid_run_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_run_07, *kid_run_08, *kid_run_09, *kid_run_10,
  *kid_run_11, *kid_run_12, *kid_run_13, *kid_run_14;

static void
init_kid_run_frameset (void)
{
  struct frameset frameset[KID_RUN_FRAMESET_NMEMB] =
    {{kid_run_07,-10,0},{kid_run_08,-7,0},{kid_run_09,-6,0},
     {kid_run_10,-4,0},{kid_run_11,-8,0},{kid_run_12,-11,0},
     {kid_run_13,-4,0},{kid_run_14,-8,0}};

  memcpy (&kid_run_frameset, &frameset,
          KID_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_run (void)
{
  /* bitmaps */
  kid_run_07 = load_bitmap (KID_RUN_07);
  kid_run_08 = load_bitmap (KID_RUN_08);
  kid_run_09 = load_bitmap (KID_RUN_09);
  kid_run_10 = load_bitmap (KID_RUN_10);
  kid_run_11 = load_bitmap (KID_RUN_11);
  kid_run_12 = load_bitmap (KID_RUN_12);
  kid_run_13 = load_bitmap (KID_RUN_13);
  kid_run_14 = load_bitmap (KID_RUN_14);

  /* frameset */
  init_kid_run_frameset ();
}

void
unload_kid_run (void)
{
  al_destroy_bitmap (kid_run_07);
  al_destroy_bitmap (kid_run_08);
  al_destroy_bitmap (kid_run_09);
  al_destroy_bitmap (kid_run_10);
  al_destroy_bitmap (kid_run_11);
  al_destroy_bitmap (kid_run_12);
  al_destroy_bitmap (kid_run_13);
  al_destroy_bitmap (kid_run_14);
}

void
kid_run (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_run) k->i = -1;

  bool stop = ! ((k->f.dir == RIGHT) ? k->key.right : k->key.left);
  bool couch = k->key.down;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);

  if (couch) {
    kid_couch (k);
    return false;
  }

  if (jump && k->f.b != kid_run_jump_frameset[10].frame) {
    kid_run_jump (k);
    return false;
  }

  if ((stop && k->f.b != kid_run_jump_frameset[10].frame)) {
    kid_stop_run (k);
    return false;
  }

  if (k->i == 7) k->i = -1;

  if (k->f.b == kid_turn_run_frameset[8].frame) k->i = 6;

  select_frame (k, kid_run_frameset, k->i + 1);

  if (k->f.b == kid_start_run_frameset[5].frame) k->fo.dx = -6;
  if (k->f.b == kid_turn_run_frameset[8].frame) k->fo.dx = -4;
  if (k->f.b == kid_run_jump_frameset[10].frame) k->fo.dx = -15;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, ptf;

  /* inertia */
  k->inertia = 0;
  k->cinertia = 6;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)) {
    kid_stabilize_collision (k);
    return false;
  }

  /* fall */
  survey (_tf, pos, &k->f, &nc, &ptf, &np);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  /* depressible floors */
  if (k->i == 2) update_depressible_floor (k, -7, -13);
  else if (k->i == 5) update_depressible_floor (k, -5, -30);
  else if (k->i == 6) update_depressible_floor (k, -4, -11);
  else keep_depressible_floor (k);

  /* sound */
  if (k->oaction == kid_run_jump)
    play_sample (step_sample, k->f.c.room);
  if (k->i == 2 || k->i == 6)
    play_sample (step_sample, k->f.c.room);
}

bool
is_kid_run (struct frame *f)
{
  int i;
  for (i = 0; i < KID_RUN_FRAMESET_NMEMB; i++)
    if (f->b == kid_run_frameset[i].frame) return true;
  return false;
}
