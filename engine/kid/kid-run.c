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
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

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
kid_run (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_run;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_run) kid->i = -1;

  bool stop = ! ((kid->f.dir == RIGHT) ? right_key : left_key);
  bool couch = down_key;
  bool jump = ((kid->f.dir == RIGHT) && right_key && up_key)
    || ((kid->f.dir == LEFT) && left_key && up_key);

  if (couch && kid == current_kid) {
    kid_couch (kid);
    return false;
  }

  if (jump && kid->f.b != kid_run_jump_frameset[10].frame
      && kid == current_kid) {
    kid_run_jump (kid);
    return false;
  }

  if ((stop && kid->f.b != kid_run_jump_frameset[10].frame)
      || kid != current_kid) {
    kid_stop_run (kid);
    return false;
  }

  if (kid->i == 7) kid->i = -1;

  if (kid->f.b == kid_turn_run_frameset[8].frame) kid->i = 6;

  select_frame (kid, kid_run_frameset, kid->i + 1);

  if (kid->f.b == kid_start_run_frameset[5].frame) kid->fo.dx = -6;
  if (kid->f.b == kid_turn_run_frameset[8].frame) kid->fo.dx = -4;
  if (kid->f.b == kid_run_jump_frameset[10].frame) kid->fo.dx = -15;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np, ptf;

  /* inertia */
  kid->inertia = 0;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, false, &kid->ci)) {
    kid_stabilize_collision (kid);
    return false;
  }

  /* fall */
  survey (_tf, pos, &kid->f, &nc, &ptf, &np);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 2) update_depressible_floor (kid, -7, -13);
  else if (kid->i == 5) clear_depressible_floor (kid);
  else if (kid->i == 6) update_depressible_floor (kid, -4, -11);
  else keep_depressible_floor (kid);

  /* sound */
  if (kid->oaction == kid_run_jump) sample_step = true;
  if (kid->i == 2 || kid->i == 6) sample_step = true;
}

bool
is_kid_run (struct frame *f)
{
  int i;
  for (i = 0; i < KID_RUN_FRAMESET_NMEMB; i++)
    if (f->b == kid_run_frameset[i].frame) return true;
  return false;
}
