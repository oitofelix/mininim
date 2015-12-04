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

struct frameset kid_start_run_frameset[KID_START_RUN_FRAMESET_NMEMB];

static void init_kid_start_run_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_start_run_01, *kid_start_run_02, *kid_start_run_03,
  *kid_start_run_04, *kid_start_run_05, *kid_start_run_06;

static void
init_kid_start_run_frameset (void)
{
  struct frameset frameset[KID_START_RUN_FRAMESET_NMEMB] =
    {{kid_start_run_01,-1,0},{kid_start_run_02,-2,0},
     {kid_start_run_03,-5,0},{kid_start_run_04,-1,0},
     {kid_start_run_05,-7,0},{kid_start_run_06,-6,0}};

  memcpy (&kid_start_run_frameset, &frameset,
          KID_START_RUN_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_start_run (void)
{
  /* bitmaps */
  kid_start_run_01 = load_bitmap (KID_START_RUN_01);
  kid_start_run_02 = load_bitmap (KID_START_RUN_02);
  kid_start_run_03 = load_bitmap (KID_START_RUN_03);
  kid_start_run_04 = load_bitmap (KID_START_RUN_04);
  kid_start_run_05 = load_bitmap (KID_START_RUN_05);
  kid_start_run_06 = load_bitmap (KID_START_RUN_06);

  /* frameset */
  init_kid_start_run_frameset ();
}

void
unload_kid_start_run (void)
{
  al_destroy_bitmap (kid_start_run_01);
  al_destroy_bitmap (kid_start_run_02);
  al_destroy_bitmap (kid_start_run_03);
  al_destroy_bitmap (kid_start_run_04);
  al_destroy_bitmap (kid_start_run_05);
  al_destroy_bitmap (kid_start_run_06);
}

void
kid_start_run (void)
{
  kid.oaction = kid.action;
  kid.action = kid_start_run;
  kid.f.flip = (kid.f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_start_run) kid->i = -1, misstep = false;

  bool run = (kid->f.dir == RIGHT) ? right_key : left_key;
  bool turn_run = (kid->f.dir == RIGHT) ? left_key : right_key;
  bool couch = down_key;
  bool jump = ((kid->f.dir == RIGHT) && right_key && up_key)
    || ((kid->f.dir == LEFT) && left_key && up_key);

  if (couch) {
    kid_couch ();
    return false;
  }

  if (jump && kid->i < 3) {
    kid_jump ();
    return false;
  }

  if ((kid->i == 3 || kid->i == 4) && turn_run) {
    kid_stop_run ();
    return false;
  }

  if (kid->i == 5) {
    if (run) kid_run ();
    else kid_stop_run ();
    return false;
  }

  select_frame (kid, kid_start_run_frameset, kid->i + 1);

  if (kid->f.b == kid_turn_frameset[3].frame) kid->fo.dx = +0;
  if (kid->f.b == kid_stabilize_frameset[0].frame) kid->fo.dx = +2;
  if (kid->f.b == kid_stabilize_frameset[1].frame) kid->fo.dx = +6;
  if (kid->f.b == kid_stabilize_frameset[2].frame) kid->fo.dx = +4;
  if (kid->f.b == kid_stabilize_frameset[3].frame) kid->fo.dx = +0;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cmbo, ctf;

  /* inertia */
  inertia = 1;

  /* fall */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &np)->fg;
  ctf = survey (_tf, pos, &kid->f, &nc, &np, &np)->fg;
  if (cmbo == NO_FLOOR || ctf == NO_FLOOR) {
    kid_fall ();
    return false;
  }

  /* collision */
  if (is_colliding (&kid->f, _tf, pos, 0, false, -kid->fo.dx)) {
    kid_stabilize_collision ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->i == 5) update_depressible_floor (kid, -16, -19);
  else keep_depressible_floor (kid);
}
