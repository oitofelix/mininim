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
#include "engine/mirror.h"
#include "kid.h"

struct frameset kid_run_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

static void init_kid_run_jump_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

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
kid_run_jump (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_run_jump;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  struct coord nc; struct pos np, pm, ptf;

  if (k->oaction != kid_run_jump)
    k->i = -1, k->hang = false;

  bool hang_front = ((k->f.dir == LEFT) ? k->key.left : k->key.right)
    && ! k->key.up && k->key.shift;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift;

  int back_dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  /* hang front */
  survey (_m, pos, &k->f, &nc, &pm, &np);
  if (hang_front && k->i >= 6  && k->i <= 9
      && is_hangable_pos (&pm, k->f.dir)) {
    k->hang_pos = pm;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_hang (k);
    return false;
  }

  /* hang back */
  survey (_tf, pos, &k->f, &nc, &ptf, &np);
  if (k->i >= 6 && k->i <= 9
      && hang_back && is_hangable_pos (&ptf, back_dir)) {
    k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_turn (k);
    return false;
  }

  if (k->i == 10) {
    kid_run (k);
    return false;
  }

  select_frame (k, kid_run_jump_frameset, k->i + 1);

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbf;

  /* inertia */
  if (k->i < 6 || k->i > 10) k->inertia = 3;
  else k->inertia = 5;

  /* collision */
  bool colliding =
    is_colliding (&k->f, &k->fo, +0, false, &k->ci);
  bool cross_mirror =
    k->ci.t == MIRROR && k->i >= 5 && k->i <= 8;

  if (colliding && ! cross_mirror) {
    if (k->i < 6) kid_stabilize_collision (k);
    else kid_couch_collision (k);
    return false;
  } else if (cross_mirror) {
    play_sample (mirror_sample, k->f.c.room);
    struct pos p; prel (&k->ci.p, &p, +0, k->f.dir == LEFT
                        ? +1 : +0);
    mirror_at_pos (&p)->kid_crossing = k->id;
  }

  /* fall */
  survey (_bf, pos, &k->f, &nc, &pbf, &np);
  if ((is_strictly_traversable (&pbf) && k->i < 4)
      || (is_strictly_traversable (&pbf) && k->i > 9)) {
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
  if (k->i == 0) update_depressible_floor (k, -4, -10);
  else if (k->i == 3) update_depressible_floor (k, -1, -4);
  else if (k->i == 6) clear_depressible_floor (k);
  else if (k->i == 10) update_depressible_floor (k, -9, -10);
  else keep_depressible_floor (k);

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  if (k->i == 10) shake_loose_floor_row (&pmbo);

  /* sound */
  if (k->i == 0 || k->i == 4) play_sample (step_sample, k->f.c.room);
}

bool
is_kid_run_jump_running (struct frame *f)
{
  int i;
  for (i = 0; i < 5; i++)
    if (f->b == kid_run_jump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_run_jump_landing (struct frame *f)
{
  int i;
  for (i = 9; i < KID_RUN_JUMP_FRAMESET_NMEMB; i++)
    if (f->b == kid_run_jump_frameset[i].frame) return true;
  return false;
}
