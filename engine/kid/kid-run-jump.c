/*
  kid-run-jump.c -- kid run jump module;

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

struct frameset kid_run_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

static void init_kid_run_jump_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_run_jump_01, *kid_run_jump_02, *kid_run_jump_03,
  *kid_run_jump_04, *kid_run_jump_05, *kid_run_jump_06,
  *kid_run_jump_07, *kid_run_jump_08, *kid_run_jump_09,
  *kid_run_jump_10, *kid_run_jump_11;

static void
init_kid_run_jump_frameset (void)
{
  struct frameset frameset[KID_RUN_JUMP_FRAMESET_NMEMB] =
    {{kid_run_jump_01,-10,+0},{kid_run_jump_02,-11,+0},
     {kid_run_jump_03,-13,+0},{kid_run_jump_04,-7,+0},
     {kid_run_jump_05,-12,+0},{kid_run_jump_06,-15,+0},
     {kid_run_jump_07,-29,-3},{kid_run_jump_08,-17,-9},
     {kid_run_jump_09,-18,-2},{kid_run_jump_10,-10,+11},
     {kid_run_jump_11,-8,+3}};

  memcpy (&kid_run_jump_frameset, &frameset,
          KID_RUN_JUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_run_jump (void)
{
  /* bitmaps */
  kid_run_jump_01 = load_bitmap (KID_RUN_JUMP_01);
  kid_run_jump_02 = load_bitmap (KID_RUN_JUMP_02);
  kid_run_jump_03 = load_bitmap (KID_RUN_JUMP_03);
  kid_run_jump_04 = load_bitmap (KID_RUN_JUMP_04);
  kid_run_jump_05 = load_bitmap (KID_RUN_JUMP_05);
  kid_run_jump_06 = load_bitmap (KID_RUN_JUMP_06);
  kid_run_jump_07 = load_bitmap (KID_RUN_JUMP_07);
  kid_run_jump_08 = load_bitmap (KID_RUN_JUMP_08);
  kid_run_jump_09 = load_bitmap (KID_RUN_JUMP_09);
  kid_run_jump_10 = load_bitmap (KID_RUN_JUMP_10);
  kid_run_jump_11 = load_bitmap (KID_RUN_JUMP_11);

  /* frameset */
  init_kid_run_jump_frameset ();
}

void
unload_kid_run_jump (void)
{
  al_destroy_bitmap (kid_run_jump_01);
  al_destroy_bitmap (kid_run_jump_02);
  al_destroy_bitmap (kid_run_jump_03);
  al_destroy_bitmap (kid_run_jump_04);
  al_destroy_bitmap (kid_run_jump_05);
  al_destroy_bitmap (kid_run_jump_06);
  al_destroy_bitmap (kid_run_jump_07);
  al_destroy_bitmap (kid_run_jump_08);
  al_destroy_bitmap (kid_run_jump_09);
  al_destroy_bitmap (kid_run_jump_10);
  al_destroy_bitmap (kid_run_jump_11);
}

void
kid_run_jump (void)
{
  kid.oaction = kid.action;
  kid.action = kid_run_jump;
  kid.f.flip = (kid.f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (&kid)) return;
  if (! physics_in (&kid)) return;
  next_frame_fo (&kid.f, &kid.f, &kid.fo);
  physics_out (&kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_run_jump) kid->i = -1;

  if (kid->i == 10) {
    kid_run ();
    return false;
  }

  select_frame (kid, kid_run_jump_frameset, kid->i + 1);

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cbf;

  /* inertia */
  if (kid->i < 6 || kid->i > 10) inertia = 3;
  else inertia = 5;

  /* fall */
  cbf = survey (_bf, pos, &kid->f, &nc, &np, &np)->fg;
  if ((cbf == NO_FLOOR && kid->i < 4)
      || (cbf == NO_FLOOR && kid->i > 9)) {
    kid_fall ();
    return false;
  }

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, false)) {
    if (kid->i < 6) kid_stabilize_collision ();
    else kid_couch_collision ();
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct coord nc; struct pos pmbo, np;

  /* depressible floors */
  if (kid->i == 0) update_depressible_floor (kid, -4, -10);
  else if (kid->i == 3) update_depressible_floor (kid, -1, -4);
  else if (kid->i == 6) clear_depressible_floor (kid);
  else if (kid->i == 10) update_depressible_floor (kid, -9, -10);
  else keep_depressible_floor (kid);

  /* loose floor shaking */
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  if (kid->i == 10) shake_loose_floor_row (&pmbo);

  /* sound */
  if (kid->i == 0 || kid->i == 4) sample_step = true;
}
