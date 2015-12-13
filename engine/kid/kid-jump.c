/*
  kid-jump.c -- kid jump module;

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

struct frameset kid_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

static void init_kid_jump_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_jump_01, *kid_jump_02, *kid_jump_03, *kid_jump_04,
  *kid_jump_05, *kid_jump_06, *kid_jump_07, *kid_jump_08, *kid_jump_09,
  *kid_jump_10, *kid_jump_11, *kid_jump_12, *kid_jump_13, *kid_jump_14,
  *kid_jump_15, *kid_jump_16, *kid_jump_17, *kid_jump_18;

static void
init_kid_jump_frameset (void)
{
  struct frameset frameset[KID_JUMP_FRAMESET_NMEMB] =
    {{kid_jump_01,+0,0},{kid_jump_02,-2,0},{kid_jump_03,-3,0},
     {kid_jump_04,-6,0},{kid_jump_05,-2,0},{kid_jump_06,-4,0},
     {kid_jump_07,-1,0},{kid_jump_08,-12,0},{kid_jump_09,-19,0},
     {kid_jump_10,-16,-6},{kid_jump_11,-2,+6},{kid_jump_12,-11,0},
     {kid_jump_13,+5,0},{kid_jump_14,-13,0},{kid_jump_15,+0,0},
     {kid_jump_16,-1,0},{kid_jump_17,-1,0},{kid_jump_18,+0,0}};

  memcpy (&kid_jump_frameset, &frameset,
          KID_JUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_jump (void)
{
  /* bitmaps */
  kid_jump_01 = load_bitmap (KID_JUMP_01);
  kid_jump_02 = load_bitmap (KID_JUMP_02);
  kid_jump_03 = load_bitmap (KID_JUMP_03);
  kid_jump_04 = load_bitmap (KID_JUMP_04);
  kid_jump_05 = load_bitmap (KID_JUMP_05);
  kid_jump_06 = load_bitmap (KID_JUMP_06);
  kid_jump_07 = load_bitmap (KID_JUMP_07);
  kid_jump_08 = load_bitmap (KID_JUMP_08);
  kid_jump_09 = load_bitmap (KID_JUMP_09);
  kid_jump_10 = load_bitmap (KID_JUMP_10);
  kid_jump_11 = load_bitmap (KID_JUMP_11);
  kid_jump_12 = load_bitmap (KID_JUMP_12);
  kid_jump_13 = load_bitmap (KID_JUMP_13);
  kid_jump_14 = load_bitmap (KID_JUMP_14);
  kid_jump_15 = load_bitmap (KID_JUMP_15);
  kid_jump_16 = load_bitmap (KID_JUMP_16);
  kid_jump_17 = load_bitmap (KID_JUMP_17);
  kid_jump_18 = load_bitmap (KID_JUMP_18);

  /* frameset */
  init_kid_jump_frameset ();
}

void
unload_kid_jump (void)
{
  al_destroy_bitmap (kid_jump_01);
  al_destroy_bitmap (kid_jump_02);
  al_destroy_bitmap (kid_jump_03);
  al_destroy_bitmap (kid_jump_04);
  al_destroy_bitmap (kid_jump_05);
  al_destroy_bitmap (kid_jump_06);
  al_destroy_bitmap (kid_jump_07);
  al_destroy_bitmap (kid_jump_08);
  al_destroy_bitmap (kid_jump_09);
  al_destroy_bitmap (kid_jump_10);
  al_destroy_bitmap (kid_jump_11);
  al_destroy_bitmap (kid_jump_12);
  al_destroy_bitmap (kid_jump_13);
  al_destroy_bitmap (kid_jump_14);
  al_destroy_bitmap (kid_jump_15);
  al_destroy_bitmap (kid_jump_16);
  al_destroy_bitmap (kid_jump_17);
  al_destroy_bitmap (kid_jump_18);
}

void
kid_jump (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_jump;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  struct coord nc; struct pos np, pm, ptf;

  if (kid->oaction != kid_jump)
    kid->i = -1, kid->misstep = kid->hang = false;

  bool hang_front = ((kid->f.dir == LEFT) ? left_key : right_key)
    && ! up_key && shift_key;

  bool hang_back = ((kid->f.dir == LEFT) ? right_key : left_key)
    && ! up_key && shift_key;

  int back_dir = (kid->f.dir == LEFT) ? RIGHT : LEFT;

  /* hang front */
  survey (_m, pos, &kid->f, &nc, &pm, &np);
  if (kid->i >= 8 && kid->i <= 10
      && hang_front && is_hangable_pos (&pm, kid->f.dir)
      && kid == current_kid) {
    kid->hang_pos = pm;
    pos2view (&kid->hang_pos, &kid->hang_pos);
    kid->hang = true;
    sample_hang_on_fall = true;
    kid_hang (kid);
    return false;
  }

  /* hang back */
  survey (_tf, pos, &kid->f, &nc, &ptf, &np);
  if (kid->i >= 8 && kid->i <= 10
      && hang_back && is_hangable_pos (&ptf, back_dir)
      && kid == current_kid) {
    kid->hang_pos = ptf;
    pos2view (&kid->hang_pos, &kid->hang_pos);
    kid->hang = true;
    sample_hang_on_fall = true;
    kid_turn (kid);
    return false;
  }

  if (kid->i == 17) {
    kid_normal (kid);
    return false;
  }

  select_frame (kid, kid_jump_frameset, kid->i + 1);

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
  enum confg cbb, cmbo, cbf;
  struct frame nf;

  /* inertia */
  if (kid->i >= 8 && kid->i <= 10) kid->inertia = 5;
  else kid->inertia = 0;

  /* collision */
  if (is_colliding (&kid->f, &kid->fo, +0, false)) {
    if (kid->i < 7 || kid->i > 10) kid_stabilize_collision (kid);
    else kid_couch_collision (kid);
    return false;
  }

  /* fall */
  next_frame (&kid->f, &nf, &kid->fo);
  cbb = survey (_bb, pos, &nf, &nc, &np, &np)->fg;
  cmbo = survey (_mbo, pos, &nf, &nc, &np, &np)->fg;
  cbf = survey (_bf, pos, &nf, &nc, &np, &np)->fg;
  if ((cbb == NO_FLOOR && cmbo == NO_FLOOR && kid->i <= 7)
      || (kid->i >= 10 && cbf == NO_FLOOR)) {
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct coord nc; struct pos np, pmbo;

  /* depressible floors */
  if (kid->i == 8) clear_depressible_floor (kid);
  else if (kid->i == 11) update_depressible_floor (kid, -11, -15);
  else if (kid->i == 14) update_depressible_floor (kid, 0, -6);
  else keep_depressible_floor (kid);

  /* loose floor shaking */
  survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
  if (kid->i == 12) shake_loose_floor_row (&pmbo);

  /* sound */
  if (kid->i == 11 || kid->i == 14) sample_step = true;
}
