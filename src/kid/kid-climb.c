/*
  kid-climb.c -- kid climb module;

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

struct frameset kid_climb_frameset[KID_CLIMB_FRAMESET_NMEMB];

static void init_kid_climb_frameset (void);
static bool flow (struct anim *k);
static bool physics_in (struct anim *k);
static void physics_out (struct anim *k);

ALLEGRO_BITMAP *kid_climb_00, *kid_climb_01, *kid_climb_02,
  *kid_climb_03, *kid_climb_04, *kid_climb_05, *kid_climb_06,
  *kid_climb_07, *kid_climb_08, *kid_climb_09, *kid_climb_10,
  *kid_climb_11, *kid_climb_12, *kid_climb_13, *kid_climb_14;

static void
init_kid_climb_frameset (void)
{
  struct frameset frameset[KID_CLIMB_FRAMESET_NMEMB] =
    {{kid_climb_00,+1,+0},{kid_climb_01,+0,-9},{kid_climb_02,-4,-5},
     {kid_climb_03,-8,-6},{kid_climb_04,-5,-4},{kid_climb_05,-2,-5},
     {kid_climb_06,-1,-5},{kid_climb_07,-4,-8},{kid_climb_08,+0,-4},
     {kid_climb_09,+0,-1},{kid_climb_10,-3,-4},{kid_climb_11,+1,+0},
     {kid_climb_12,+0,+0},{kid_climb_13,-1,+0},{kid_climb_14,+0,+0}};

  memcpy (&kid_climb_frameset, &frameset,
          KID_CLIMB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_climb (void)
{
  /* bitmaps */
  kid_climb_00 = load_bitmap (KID_CLIMB_00);
  kid_climb_01 = load_bitmap (KID_CLIMB_01);
  kid_climb_02 = load_bitmap (KID_CLIMB_02);
  kid_climb_03 = load_bitmap (KID_CLIMB_03);
  kid_climb_04 = load_bitmap (KID_CLIMB_04);
  kid_climb_05 = load_bitmap (KID_CLIMB_05);
  kid_climb_06 = load_bitmap (KID_CLIMB_06);
  kid_climb_07 = load_bitmap (KID_CLIMB_07);
  kid_climb_08 = load_bitmap (KID_CLIMB_08);
  kid_climb_09 = load_bitmap (KID_CLIMB_09);
  kid_climb_10 = load_bitmap (KID_CLIMB_10);
  kid_climb_11 = load_bitmap (KID_CLIMB_11);
  kid_climb_12 = load_bitmap (KID_CLIMB_12);
  kid_climb_13 = load_bitmap (KID_CLIMB_13);
  kid_climb_14 = load_bitmap (KID_CLIMB_14);

  /* frameset */
  init_kid_climb_frameset ();
}

void
unload_kid_climb (void)
{
  destroy_bitmap (kid_climb_00);
  destroy_bitmap (kid_climb_01);
  destroy_bitmap (kid_climb_02);
  destroy_bitmap (kid_climb_03);
  destroy_bitmap (kid_climb_04);
  destroy_bitmap (kid_climb_05);
  destroy_bitmap (kid_climb_06);
  destroy_bitmap (kid_climb_07);
  destroy_bitmap (kid_climb_08);
  destroy_bitmap (kid_climb_09);
  destroy_bitmap (kid_climb_10);
  destroy_bitmap (kid_climb_11);
  destroy_bitmap (kid_climb_12);
  destroy_bitmap (kid_climb_13);
  destroy_bitmap (kid_climb_14);
}

void
kid_climb (struct anim *k)
{
  k->oaction = k->action;
  k->action = kid_climb;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct anim *k)
{
  if (k->oaction != kid_climb) {
    k->i = -1;
    k->wait = DOOR_WAIT_LOOK;
  }

  struct pos hanged_pos;
  get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);

  if (k->i == 14) {
    kid_couch (k);
    return false;
  }

  if (k->i == -1) {
    int dir = (k->f.dir == LEFT) ? 0 : 1;
    place_frame (&k->f, &k->f, kid_climb_frameset[0].frame,
                 &k->hang_pos, PLACE_WIDTH * dir + 9, -9);
  }

  if (k->wait == DOOR_WAIT_LOOK && k->i < 14) k->i++;

  select_frame (k, kid_climb_frameset, k->i);

  /* climbing when looking left should let the kid near to the edge
     if it's not a door */
  if (k->f.dir == LEFT && k->fo.dx != 0
      && k->i % 2
      && con (&hanged_pos)->fg != DOOR)
    k->fo.dx += 1;

  if (k->i == 3 && k->wait < DOOR_WAIT_LOOK)
    k->fo.dx = 0, k->fo.dy = 0;

  return true;
}

static bool
physics_in (struct anim *k)
{
  struct pos ptf;
  enum confg ctf;

  /* fall */
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);
  if (is_strictly_traversable (&ptf)) {
    kid_fall (k);
    return false;
  }

  /* door collision */
  ctf = survey (_tf, pos, &k->f, NULL, &ptf, NULL)->fg;
  if (k->i == 3 && ctf == DOOR && k->f.dir == LEFT
      && door_at_pos (&ptf)->i > DOOR_CLIMB_LIMIT) {
    if (k->wait == 0) {
      k->hang_limit = true;
      kid_unclimb (k);
      return false;
    } else if (k->wait > 0) k->wait--;
  }

  return true;
}

static void
physics_out (struct anim *k)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (k);
  get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}

bool
is_kid_climb (struct frame *f)
{
  int i;
  for (i = 0; i < 10; i++)
    if (f->b == kid_climb_frameset[i].frame) return true;
  return false;
}

bool
is_kid_successfully_climbing_at_pos (struct frame *f, struct pos *hang_pos,
                                     struct pos *p)
{
  struct pos np;
  int dir = (f->dir == LEFT) ? +1 : -1;
  return is_kid_successfully_climbing (f)
    && peq (prel (p, &np, +1, dir), hang_pos);
}

bool
is_kid_successfully_climbing (struct frame *f)
{
  int i;
  for (i = 4; i < KID_CLIMB_FRAMESET_NMEMB; i++)
    if (f->b == kid_climb_frameset[i].frame) return true;
  return false;
}
