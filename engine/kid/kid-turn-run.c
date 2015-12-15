/*
  kid-turn-run.c -- kid turn run module;

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

struct frameset kid_turn_run_frameset[KID_TURN_RUN_FRAMESET_NMEMB];

static void init_kid_turn_run_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_turn_run_05, *kid_turn_run_06, *kid_turn_run_07, *kid_turn_run_08,
  *kid_turn_run_09, *kid_turn_run_10, *kid_turn_run_11, *kid_turn_run_12,
  *kid_turn_run_13;

static void
init_kid_turn_run_frameset (void)
{
  struct frameset frameset[KID_TURN_RUN_FRAMESET_NMEMB] =
    {{kid_turn_run_05,-18,0},{kid_turn_run_06,-6,0},{kid_turn_run_07,-4,0},
     {kid_turn_run_08,+2,0},{kid_turn_run_09,-6,0},{kid_turn_run_10,+3,0},
     {kid_turn_run_11,-1,0},{kid_turn_run_12,+0,0},{kid_turn_run_13,+4,0}};

  memcpy (&kid_turn_run_frameset, &frameset,
          KID_TURN_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_turn_run (void)
{
  /* bitmaps */
  kid_turn_run_05 = load_bitmap (KID_TURN_RUN_05);
  kid_turn_run_06 = load_bitmap (KID_TURN_RUN_06);
  kid_turn_run_07 = load_bitmap (KID_TURN_RUN_07);
  kid_turn_run_08 = load_bitmap (KID_TURN_RUN_08);
  kid_turn_run_09 = load_bitmap (KID_TURN_RUN_09);
  kid_turn_run_10 = load_bitmap (KID_TURN_RUN_10);
  kid_turn_run_11 = load_bitmap (KID_TURN_RUN_11);
  kid_turn_run_12 = load_bitmap (KID_TURN_RUN_12);
  kid_turn_run_13 = load_bitmap (KID_TURN_RUN_13);

  /* frameset */
  init_kid_turn_run_frameset ();
}

void
unload_kid_turn_run (void)
{
  al_destroy_bitmap (kid_turn_run_05);
  al_destroy_bitmap (kid_turn_run_06);
  al_destroy_bitmap (kid_turn_run_07);
  al_destroy_bitmap (kid_turn_run_08);
  al_destroy_bitmap (kid_turn_run_09);
  al_destroy_bitmap (kid_turn_run_10);
  al_destroy_bitmap (kid_turn_run_11);
  al_destroy_bitmap (kid_turn_run_12);
  al_destroy_bitmap (kid_turn_run_13);
}

void
kid_turn_run (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_turn_run;

  if (kid->oaction != kid_turn_run)
    kid->f.dir = (kid->f.dir == LEFT) ? RIGHT : LEFT;
  kid->f.flip = (kid->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

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
  if (kid->oaction != kid_turn_run) kid->i = -1;

  if (kid->i == 8) {
    kid_run (kid);
    return false;
  }

  select_frame (kid, kid_turn_run_frameset, kid->i + 1);
  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg ctb;

  /* inertia */
  kid->inertia = 0;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, true, &kid->ci)) {
    kid->f.dir = (kid->f.dir == LEFT) ? RIGHT : LEFT;
    kid_stabilize_collision (kid);
    return false;
  }

  /* fall */
  ctb = survey (_tb, pos, &kid->f, &nc, &np, &np)->fg;
  if (ctb == NO_FLOOR) {
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 0) update_depressible_floor (kid, -4, -27);
  else if (kid->i == 1) update_depressible_floor (kid, -9, -28);
  else if (kid->i == 2) update_depressible_floor (kid, -11, -29);
  else if (kid->i == 3) update_depressible_floor (kid, -6, -27);
  else if (kid->i == 4) update_depressible_floor (kid, -10, -31);
  else if (kid->i == 5) update_depressible_floor (kid, -17, -29);
  else keep_depressible_floor (kid);
}

bool
is_kid_turn_run (struct frame *f)
{
  int i;
  for (i = 0; i < KID_TURN_RUN_FRAMESET_NMEMB; i++)
    if (f->b == kid_turn_run_frameset[i].frame) return true;
  return false;
}
