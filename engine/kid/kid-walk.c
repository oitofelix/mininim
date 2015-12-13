/*
  kid-walk.c -- kid walk module;

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

struct frameset kid_walk_frameset[KID_WALK_FRAMESET_NMEMB];

static void init_kid_walk_frameset (void);
static bool flow (struct anim *kid);
static bool physics_in (struct anim *kid);
static void physics_out (struct anim *kid);

ALLEGRO_BITMAP *kid_walk_01, *kid_walk_02, *kid_walk_03,
  *kid_walk_04, *kid_walk_05, *kid_walk_06, *kid_walk_07,
  *kid_walk_08, *kid_walk_09, *kid_walk_10, *kid_walk_11,
  *kid_walk_12;

static void
init_kid_walk_frameset (void)
{
  struct frameset frameset[KID_WALK_FRAMESET_NMEMB] =
    {{kid_walk_01,-1,0},{kid_walk_02,-1,0},{kid_walk_03,+0,0},
     {kid_walk_04,-8,0},{kid_walk_05,-7,0},{kid_walk_06,-6,0},
     {kid_walk_07,+3,0},{kid_walk_08,-2,0},{kid_walk_09,-1,0},
     {kid_walk_10,-1,0},{kid_walk_11,-2,0},{kid_walk_12,+0,0}};

  memcpy (&kid_walk_frameset, &frameset,
          KID_WALK_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
load_kid_walk (void)
{
  /* bitmaps */
  kid_walk_01 = load_bitmap (KID_WALK_01);
  kid_walk_02 = load_bitmap (KID_WALK_02);
  kid_walk_03 = load_bitmap (KID_WALK_03);
  kid_walk_04 = load_bitmap (KID_WALK_04);
  kid_walk_05 = load_bitmap (KID_WALK_05);
  kid_walk_06 = load_bitmap (KID_WALK_06);
  kid_walk_07 = load_bitmap (KID_WALK_07);
  kid_walk_08 = load_bitmap (KID_WALK_08);
  kid_walk_09 = load_bitmap (KID_WALK_09);
  kid_walk_10 = load_bitmap (KID_WALK_10);
  kid_walk_11 = load_bitmap (KID_WALK_11);
  kid_walk_12 = load_bitmap (KID_WALK_12);

  /* frameset */
  init_kid_walk_frameset ();
}

void
unload_kid_walk (void)
{
  al_destroy_bitmap (kid_walk_01);
  al_destroy_bitmap (kid_walk_02);
  al_destroy_bitmap (kid_walk_03);
  al_destroy_bitmap (kid_walk_04);
  al_destroy_bitmap (kid_walk_05);
  al_destroy_bitmap (kid_walk_06);
  al_destroy_bitmap (kid_walk_07);
  al_destroy_bitmap (kid_walk_08);
  al_destroy_bitmap (kid_walk_09);
  al_destroy_bitmap (kid_walk_10);
  al_destroy_bitmap (kid_walk_11);
  al_destroy_bitmap (kid_walk_12);
}

void
kid_walk (struct anim *kid)
{
  kid->oaction = kid->action;
  kid->action = kid_walk;
  kid->f.flip = (kid->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (kid)) return;
  if (! physics_in (kid)) return;
  next_frame (&kid->f, &kid->f, &kid->fo);
  physics_out (kid);
}

static bool
flow (struct anim *kid)
{
  struct coord nc; struct pos np, pbf, pmbo;

  if (kid->oaction != kid_walk) {
    survey (_bf, pos, &kid->f, &nc, &pbf, &np);
    survey (_mbo, pos, &kid->f, &nc, &pmbo, &np);
    if (con (&pbf)->fg == NO_FLOOR
        || con (&pbf)->fg == LOOSE_FLOOR) kid->p = pmbo;
    else kid->p = pbf;
    kid->i = kid->walk = -1;
  }

  confg_collision = NO_FLOOR;

  int dc = dist_collision (&kid->f, false) + 4;
  int df = dist_con (&kid->f, _bf, pos, -4, false, NO_FLOOR);
  int dl = dist_con (&kid->f, _bf, pos, -4, false, LOOSE_FLOOR);
  int dd = dist_con (&kid->f, _bf, pos, -4, false, CLOSER_FLOOR);

  int dcc = 0;
  if (confg_collision == DOOR) {
    dcc = 9;
    dc -= dcc;
  }

  if (kid->i == -1 && con (&kid->p)->fg != LOOSE_FLOOR) {
    if (dc < 4) {
      kid_normal (kid);
      return false;
    }

    if (! kid->misstep) {
      if (dd < 4) {
        kid_normal (kid);
        kid->misstep = true;
        return false;
      }

      if (df < 4 || dl < 4) {
        kid_misstep (kid);
        return false;
      }

      int dx = 0;
      if (dc < 10 || df < 10 || dl < 10 || dd < 10)
        kid->walk = 0, dx = 5;
      else if (dc < 15 || df < 15 || dl < 15 || dd < 15)
        kid->walk = 1, dx = 10;
      else if (dc < 22 || df < 22 || dl < 22 || dd < 22)
        kid->walk = 2, dx = 15;
      else if (dc < 27 || df < 27 || dl < 27 || dd < 27)
        kid->walk = 3, dx = 22;

      if (kid->walk != -1 )
        place_frame (&kid->f, &kid->f, kid_normal_00, &kid->p,
                     (kid->f.dir == LEFT) ? +11 + dx + dcc
                     : PLACE_WIDTH + 7 - dx - dcc, +15);
    }
  } else if (kid->i == 2 && kid->walk == 0) kid->i = 9;
  else if (kid->i == 3 && kid->walk == 1) kid->i = 8;
  else if (kid->i == 4 && kid->walk == 2) kid->i = 6;
  else if (kid->i == 5 && kid->walk == 3) kid->i = 6;
  else if (kid->i == 11){
    if (kid->walk != -1)
      place_frame (&kid->f, &kid->f, kid_normal_00, &kid->p,
                   (kid->f.dir == LEFT) ? +11 + dcc
                   : PLACE_WIDTH + 7 - dcc, +15);
    kid_normal (kid);
    kid->misstep = false;
    return false;
  }

  select_frame (kid, kid_walk_frameset, kid->i + 1);

  if (kid->f.b == kid_turn_frameset[3].frame) kid->fo.dx = +0;

  if (kid->walk == 0) {
    if (dc > 4 && df > 4 && dl > 4 && dd > 4) kid->fo.dx += -1;
  }

  if (kid->walk == 1) {
    if (kid->i == 9) kid->fo.dx = +1;
    if (kid->i == 10) kid->fo.dx = -1;
  }

  if (kid->walk == 2 || kid->walk == 3) {
    if (kid->i == 7) kid->fo.dx = +3;
    if (kid->i == 10) kid->fo.dx = -1;
  }

  return true;
}

static bool
physics_in (struct anim *kid)
{
  struct coord nc; struct pos np;
  enum confg cmbo, cbb;

  /* inertia */
  kid->inertia = 0;

  /* fall */
  cmbo = survey (_mbo, pos, &kid->f, &nc, &np, &np)->fg;
  cbb = survey (_bb, pos, &kid->f, &nc, &np, &np)->fg;
  if (kid->walk == -1
      && ((kid->i < 6 && cbb == NO_FLOOR)
          || (kid->i >= 6 && cmbo == NO_FLOOR))) {
    kid_fall (kid);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *kid)
{
  /* depressible floors */
  if (kid->walk == -1) {
    if (kid->i == 6) update_depressible_floor (kid, -3, -5);
    else if (kid->i == 7) update_depressible_floor (kid, 0, -6);
    else if (kid->i == 10) update_depressible_floor (kid, -4, -10);
    else keep_depressible_floor (kid);
  } else keep_depressible_floor (kid);
}
