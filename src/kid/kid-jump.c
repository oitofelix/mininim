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

#include "mininim.h"

struct frameset kid_jump_frameset[KID_JUMP_FRAMESET_NMEMB];

static void init_kid_jump_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_jump_00, *kid_jump_01, *kid_jump_02, *kid_jump_03,
  *kid_jump_04, *kid_jump_05, *kid_jump_06, *kid_jump_07, *kid_jump_08,
  *kid_jump_09, *kid_jump_10, *kid_jump_11, *kid_jump_12, *kid_jump_13,
  *kid_jump_14, *kid_jump_15, *kid_jump_16, *kid_jump_17;

static void
init_kid_jump_frameset (void)
{
  struct frameset frameset[KID_JUMP_FRAMESET_NMEMB] =
    {{kid_jump_00,+0,0},{kid_jump_01,-2,0},{kid_jump_02,-3,0},
     {kid_jump_03,-6,0},{kid_jump_04,-2,0},{kid_jump_05,-4,0},
     {kid_jump_06,-1,0},{kid_jump_07,-12,0},{kid_jump_08,-19,0},
     {kid_jump_09,-16,-6},{kid_jump_10,-2,+6},{kid_jump_11,-11,0},
     {kid_jump_12,+5,0},{kid_jump_13,-13,0},{kid_jump_14,+0,0},
     {kid_jump_15,-1,0},{kid_jump_16,-1,0},{kid_jump_17,+0,0}};

  memcpy (&kid_jump_frameset, &frameset,
          KID_JUMP_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_jump (void)
{
  /* bitmaps */
  kid_jump_00 = load_bitmap (KID_JUMP_00);
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

  /* frameset */
  init_kid_jump_frameset ();
}

void
unload_kid_jump (void)
{
  destroy_bitmap (kid_jump_00);
  destroy_bitmap (kid_jump_01);
  destroy_bitmap (kid_jump_02);
  destroy_bitmap (kid_jump_03);
  destroy_bitmap (kid_jump_04);
  destroy_bitmap (kid_jump_05);
  destroy_bitmap (kid_jump_06);
  destroy_bitmap (kid_jump_07);
  destroy_bitmap (kid_jump_08);
  destroy_bitmap (kid_jump_09);
  destroy_bitmap (kid_jump_10);
  destroy_bitmap (kid_jump_11);
  destroy_bitmap (kid_jump_12);
  destroy_bitmap (kid_jump_13);
  destroy_bitmap (kid_jump_14);
  destroy_bitmap (kid_jump_15);
  destroy_bitmap (kid_jump_16);
  destroy_bitmap (kid_jump_17);
}

void
kid_jump (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_jump;
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

  if (k->oaction != kid_jump)
    k->i = -1, k->misstep = k->hang = false;

  bool hang_front = ((k->f.dir == LEFT) ? k->key.left : k->key.right)
    && ! k->key.up && k->key.shift;

  bool hang_back = ((k->f.dir == LEFT) ? k->key.right : k->key.left)
    && ! k->key.up && k->key.shift;

  int back_dir = (k->f.dir == LEFT) ? RIGHT : LEFT;

  /* hang front */
  survey (_m, pos, &k->f, &nc, &pm, &np);
  survey (_tf, pos, &k->f, &nc, &ptf, &np);
  if (k->i >= 8 && k->i <= 10 && hang_front
      && (is_hangable_pos (&pm, k->f.dir)
          || is_hangable_pos (&ptf, k->f.dir))) {
    if (is_hangable_pos (&pm, k->f.dir)) k->hang_pos = pm;
    else if (is_hangable_pos (&ptf, k->f.dir)) k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_hang (k);
    return false;
  }

  /* hang back */
  survey (_tf, pos, &k->f, &nc, &ptf, &np);
  if (k->i >= 8 && k->i <= 10
      && hang_back && is_hangable_pos (&ptf, back_dir)) {
    k->hang_pos = ptf;
    pos2room (&k->hang_pos, k->f.c.room, &k->hang_pos);
    k->hang = true;
    play_sample (hang_on_fall_sample, k->f.c.room);
    kid_turn (k);
    return false;
  }

  if (k->i == 17) {
    kid_normal (k);
    return false;
  }

  select_frame (k, kid_jump_frameset, k->i + 1);

  if (k->f.b == kid_stabilize_frameset[0].frame) k->fo.dx = +2;
  if (k->f.b == kid_stabilize_frameset[1].frame) k->fo.dx = +6;
  if (k->f.b == kid_stabilize_frameset[2].frame) k->fo.dx = +4;
  if (k->f.b == kid_stabilize_frameset[3].frame) k->fo.dx = +0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct coord nc; struct pos np, pbb, pmbo, pbf;
  struct frame nf;

  /* inertia */
  if (k->i >= 8 && k->i <= 10) k->inertia = 5;
  else k->inertia = 0;

  /* collision */
  if (is_colliding (&k->f, &k->fo, +0, false, &k->ci)) {
    if (k->i < 7 || k->i > 10) kid_stabilize_collision (k);
    else kid_couch_collision (k);
    return false;
  }

  /* fall */
  next_frame (&k->f, &nf, &k->fo);
  survey (_bb, pos, &nf, &nc, &pbb, &np);
  survey (_mbo, pos, &nf, &nc, &pmbo, &np);
  survey (_bf, pos, &nf, &nc, &pbf, &np);
  if ((is_strictly_traversable (&pbb)
       && is_strictly_traversable (&pmbo) && k->i < 7)
      || (k->i >= 10 && k->i < 13
          && is_strictly_traversable (&pbf))
      || (k->i >= 13 && is_strictly_traversable (&pmbo))) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  struct coord nc; struct pos np, pmbo;

  /* depressible floors */
  if (k->i == 8) clear_depressible_floor (k);
  else if (k->i == 11) update_depressible_floor (k, -11, -15);
  else if (k->i == 14) update_depressible_floor (k, 0, -6);
  else keep_depressible_floor (k);

  /* loose floor shaking */
  survey (_mbo, pos, &k->f, &nc, &pmbo, &np);
  if (k->i == 12) shake_loose_floor_row (&pmbo);

  /* sound */
  if (k->i == 11 || k->i == 14) play_sample (step_sample, k->f.c.room);
}

bool
is_kid_jump (struct frame *f)
{
  int i;
  for (i = 0; i < KID_JUMP_FRAMESET_NMEMB; i++)
    if (f->b == kid_jump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_jump_start (struct frame *f)
{
  int i;
  for (i = 0; i < 6; i++)
    if (f->b == kid_jump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_jump_air (struct frame *f)
{
  int i;
  for (i = 6; i < 11; i++)
    if (f->b == kid_jump_frameset[i].frame) return true;
  return false;
}

bool
is_kid_jump_landing (struct frame *f)
{
  int i;
  for (i = 11; i < KID_JUMP_FRAMESET_NMEMB; i++)
    if (f->b == kid_jump_frameset[i].frame) return true;
  return false;
}
