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

struct frameset kid_climb_frameset[KID_CLIMB_FRAMESET_NMEMB];

static void init_kid_climb_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_climb_01, *kid_climb_02, *kid_climb_03,
  *kid_climb_04, *kid_climb_05, *kid_climb_06, *kid_climb_07,
  *kid_climb_08, *kid_climb_09, *kid_climb_10, *kid_climb_11,
  *kid_climb_12, *kid_climb_13, *kid_climb_14, *kid_climb_15;

static void
init_kid_climb_frameset (void)
{
  struct frameset frameset[KID_CLIMB_FRAMESET_NMEMB] =
    {{kid_climb_01,+1,+0},{kid_climb_02,+0,-9},{kid_climb_03,-4,-5},
     {kid_climb_04,-8,-6},{kid_climb_05,-5,-4},{kid_climb_06,-2,-5},
     {kid_climb_07,-1,-5},{kid_climb_08,-4,-8},{kid_climb_09,+0,-4},
     {kid_climb_10,+0,-1},{kid_climb_11,-3,-4},{kid_climb_12,+1,+0},
     {kid_climb_13,+0,+0},{kid_climb_14,-1,+0},{kid_climb_15,+0,+0}};

  memcpy (&kid_climb_frameset, &frameset,
          KID_CLIMB_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_climb (void)
{
  /* bitmaps */
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
  kid_climb_15 = load_bitmap (KID_CLIMB_15);

  /* frameset */
  init_kid_climb_frameset ();
}

void
unload_kid_climb (void)
{
  al_destroy_bitmap (kid_climb_01);
  al_destroy_bitmap (kid_climb_02);
  al_destroy_bitmap (kid_climb_03);
  al_destroy_bitmap (kid_climb_04);
  al_destroy_bitmap (kid_climb_05);
  al_destroy_bitmap (kid_climb_06);
  al_destroy_bitmap (kid_climb_07);
  al_destroy_bitmap (kid_climb_08);
  al_destroy_bitmap (kid_climb_09);
  al_destroy_bitmap (kid_climb_10);
  al_destroy_bitmap (kid_climb_11);
  al_destroy_bitmap (kid_climb_12);
  al_destroy_bitmap (kid_climb_13);
  al_destroy_bitmap (kid_climb_14);
  al_destroy_bitmap (kid_climb_15);
}

void
kid_climb (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_climb;
  kid->f.flip = (kid->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  if (kid->oaction != kid_climb) {
    kid->i = -1;
    kid->wait = DOOR_WAIT_LOOK;
  }

  struct pos hanged_pos;
  get_hanged_pos (&kid->hang_pos, kid->f.dir, &hanged_pos);

  if (kid->i == 14) {
    kid_couch (kid);
    return false;
  }

  if (kid->i == -1) {
    int dir = (kid->f.dir == LEFT) ? 0 : 1;
    place_frame (&kid->f, &kid->f, kid_climb_frameset[0].frame,
                 &kid->hang_pos, PLACE_WIDTH * dir + 9, -9);
  }

  if (kid->wait == DOOR_WAIT_LOOK && kid->i < 14) kid->i++;

  select_frame (kid, kid_climb_frameset, kid->i);

  /* climbing when looking left should let the kid near to the edge
     if it's not a door */
  if (kid->f.dir == LEFT && kid->fo.dx != 0
      && kid->i % 2
      && con (&hanged_pos)->fg != DOOR)
    kid->fo.dx += 1;

  if (kid->i == 3 && kid->wait < DOOR_WAIT_LOOK)
    kid->fo.dx = 0, kid->fo.dy = 0;

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np, ptf;
  enum confg ctf;

  /* fall */
  ctf = survey (_tf, pos, &kid->f, &nc, &np, &np)->fg;
  if (ctf == NO_FLOOR) {
    kid_fall (kid);
    return false;
  }

  /* door collision */
  survey (_tf, pos, &kid->f, &nc, &ptf, &np);
  if (kid->i == 3 && ctf == DOOR && kid->f.dir == LEFT
      && door_at_pos (&ptf)->i > DOOR_CLIMB_LIMIT) {
    if (kid->wait == 0) {
      kid->hang_limit = true;
      kid_unclimb (kid);
      return false;
    } else if (kid->wait > 0) kid->wait--;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  struct pos hanged_pos;

  /* depressible floors */
  clear_depressible_floor (kid);
  get_hanged_pos (&kid->hang_pos, kid->f.dir, &hanged_pos);
  press_depressible_floor (&hanged_pos);
}

bool
is_kid_climb (struct frame *f)
{
  int i;
  for (i = 2; i < 10; i++)
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
