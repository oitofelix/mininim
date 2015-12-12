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

#include <stdio.h>
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

struct frameset kid_stop_run_frameset[KID_STOP_RUN_FRAMESET_NMEMB];

static void init_kid_stop_run_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_stop_run_01, *kid_stop_run_02,
  *kid_stop_run_03, *kid_stop_run_04;

static void
init_kid_stop_run_frameset (void)
{
  struct frameset frameset[KID_STOP_RUN_FRAMESET_NMEMB] =
    {{kid_stop_run_01,+0,0},{kid_stop_run_02,-1,0},
     {kid_stop_run_03,-21,0},{kid_stop_run_04,-2,0}};

  memcpy (&kid_stop_run_frameset, &frameset,
          KID_STOP_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_stop_run (void)
{
  /* bitmaps */
  kid_stop_run_01 = load_bitmap (KID_STOP_RUN_01);
  kid_stop_run_02 = load_bitmap (KID_STOP_RUN_02);
  kid_stop_run_03 = load_bitmap (KID_STOP_RUN_03);
  kid_stop_run_04 = load_bitmap (KID_STOP_RUN_04);

  /* frameset */
  init_kid_stop_run_frameset ();
}

void
unload_kid_stop_run (void)
{
  al_destroy_bitmap (kid_stop_run_01);
  al_destroy_bitmap (kid_stop_run_02);
  al_destroy_bitmap (kid_stop_run_03);
  al_destroy_bitmap (kid_stop_run_04);
}

void
kid_stop_run (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_stop_run;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_stop_run) kid->i = -1;

  bool turn_run = (kid->f.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;

  if (couch) {
    kid_couch (kid);
    return false;
  }

  if (kid->i == 3) {
    if (turn_run) kid_turn_run (kid);
    else kid_stabilize (kid);
    return false;
  }

  select_frame (kid, kid_stop_run_frameset, kid->i + 1);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg ctf;

  /* inertia */
  inertia = 0;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, false)) {
    kid_stabilize_collision (kid);
    return false;
  }

  /* fall */
  ctf = survey (_tf, pos, &kid->f, &nc, &np, &np)->fg;
  if (ctf == NO_FLOOR) {
    kid_fall (kid);
    return false;
  }

  return true;
}

void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 0) update_depressible_floor (kid, -1, -5);
  else if (kid->i == 2) update_depressible_floor (kid, -2, -7);
  else keep_depressible_floor (kid);

  /* sound */
  if (kid->i == 1 || kid->i == 3) sample_step = true;
}
