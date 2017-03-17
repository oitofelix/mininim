/*
  kid-walk.c -- kid walk module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

static bool flow (struct actor *k);
static bool physics_in (struct actor *k);
static void physics_out (struct actor *k);

void
kid_walk (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_walk;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  struct pos pbf, pmbo;

  if (k->oaction != kid_walk) {
    survey (_bf, pos, &k->f, NULL, &pbf, NULL);
    survey (_mbo, pos, &k->f, NULL, &pmbo, NULL);
    if (is_potentially_traversable (&pbf) || fg (&pbf) == CLOSER_FLOOR)
      k->p = pmbo;
    else k->p = pbf;
    k->i = k->walk = -1;

    k->dc = dist_collision (&k->f, _bf, -4, -4, &k->ci);
    k->df = dist_fall (&k->f, false);
    k->dl = dist_tile (&k->f, _bf, pos, -4, false, LOOSE_FLOOR);
    k->dcl = dist_tile (&k->f, _bf, pos, -4, false, CLOSER_FLOOR);
    k->dch = dist_chomper (&k->f, false);

    struct pos pf;
    prel (&k->p, &pf, +0, (k->f.dir == LEFT) ? -1 : +1);

    k->dcd = 0;

    if (k->dc <= PLACE_WIDTH
        && k->f.dir == RIGHT) {
      k->dcd = 9;
      k->dc -= k->dcd;
    }

    if (k->dc <= PLACE_WIDTH
        && k->f.dir == LEFT
        && fg (&k->ci.tile_p) == MIRROR) {
      k->dcd = 5;
      k->dc -= k->dcd;
    }

    if (k->dch < 4 && k->dch <= k->dl && k->dch <= k->df)
      k->misstep = true;
    else if (k->dch <= PLACE_WIDTH) {
      k->dcd = 5;
      /* k->dch -= k->dcd; */
    }

    if (k->dch <= PLACE_WIDTH) k->tile_fg = CHOMPER;
    else if (k->df <= PLACE_WIDTH) k->tile_fg = NO_FLOOR;
    else if (k->dl <= PLACE_WIDTH) k->tile_fg = LOOSE_FLOOR;
    else if (k->dcl <= PLACE_WIDTH) k->tile_fg = CLOSER_FLOOR;
    else if (k->dc <= PLACE_WIDTH
             && is_valid_pos (&k->ci.tile_p))
      k->tile_fg = fg (&k->ci.tile_p);
    else k->tile_fg = FLOOR;
  }

  if (k->i == -1) {
    if (k->dc <= 5) {
      kid_normal (k);
      return false;
    }

    if (! k->misstep) {
      if (k->df < 4 || k->dl < 4 || k->dcl < 4) {
        kid_misstep (k);
        return false;
      }

      if (k->dch <= PLACE_WIDTH)
        k->misstep = true;

      int dx = 0;
      if (k->dc < 10 || k->df < 10 || k->dl < 10
          || k->dcl < 10 || k->dch < 10)
        k->walk = 0, dx = 5;
      else if (k->dc < 15 || k->df < 15 || k->dl < 15
               || k->dcl < 15 || k->dch < 15)
        k->walk = 1, dx = 10;
      else if (k->dc < 22 || k->df < 22 || k->dl < 22
               || k->dcl < 22 || k->dch < 22)
        k->walk = 2, dx = 15;
      else if (k->dc < 28 || k->df < 28 || k->dl < 28
               || k->dcl < 28 || k->dch < 28)
        k->walk = 3, dx = 22;

      if (k->walk != -1 ) {
        int pdx = (k->f.dir == LEFT) ? +11 + dx + k->dcd
          : PLACE_WIDTH + 7 - dx - k->dcd;
        int pdy = +15;
        place_actor (k, &k->p, pdx, pdy, "KID", "NORMAL", 0);
      }
    }
  } else if (k->i == 2 && k->walk == 0) k->i = 9;
  else if (k->i == 3 && k->walk == 1) k->i = 8;
  else if (k->i == 4 && k->walk == 2) k->i = 6;
  else if (k->i == 5 && k->walk == 3) k->i = 6;
  else if (k->i == 11) {
    if (k->walk != -1) {
      /* printf ("tile_fg: %s\n", get_tile_fg_name (k->tile_fg)); */

      if (k->tile_fg == CHOMPER) {
        int pdx = (k->f.dir == LEFT) ? +15 : PLACE_WIDTH + 3;
        int pdy = +15;
        place_actor (k, &k->p, pdx, pdy, "KID", "NORMAL", 0);
      }
      /* else if (k->tile_fg == MIRROR && k->f.dir == LEFT) */
      /*   place_frame (&k->f, &k->f, kid_normal_00, &k->p, */
      /*                +16 + k->dcd, +15); */
      else {
        int pdx = (k->f.dir == LEFT) ? +11 + k->dcd
          : PLACE_WIDTH + 7 - k->dcd;
        int pdy = +15;
        place_actor (k, &k->p, pdx, pdy, "KID", "NORMAL", 0);
      }
    }
    kid_normal (k);
    k->misstep = false;
    return false;
  }

  select_actor_frame (k, "KID", "WALK", k->i + 1);

  /* if (k->f.b == kid_turn_frameset[3].frame) k->fo.dx = +0; */

  if (k->walk == 0) {
    if (k->dc > 4 && k->df > 4 && k->dl > 4 && k->dcl > 4)
      k->fo.dx += -1;
    if (k->i == 10) k->fo.dx = +0;
  }

  if (k->walk == 1) {
    if (k->i == 9) k->fo.dx = +1;
    if (k->i == 10) k->fo.dx = +0;
  }

  if (k->walk == 2 || k->walk == 3) {
    if (k->i == 7) k->fo.dx = +3;
    if (k->i == 10) k->fo.dx = +0;
  }

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = k->cinertia = 0;

  /* fall */
  if (k->walk == -1
      && ((k->i < 6 && is_falling (&k->f, _bb, +0, +0)
           && is_falling (&k->f, _mbo, +0, +0))
          || (k->i >= 6 && is_falling (&k->f, _mbo, +0, +0)
              && is_falling (&k->f, _bf, +0, +0)))) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  int dx = 0;
  if (k->walk != -1) dx = -4;
  if (k->i == 6) update_depressible_floor (k, -3 + dx, -5);
  else if (k->i == 7) update_depressible_floor (k, 0 + dx, -6);
  else if (k->i == 10) update_depressible_floor (k, -4 + dx, -10);
  else keep_depressible_floor (k);
}
