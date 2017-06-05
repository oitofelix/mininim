/*
  kid-unclimb.c -- kid unclimb module;

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
kid_unclimb (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_unclimb;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  struct pos hanged_pos;

  if (k->i >= 4 && k->i <= 8)
    request_gamepad_rumble (0.8, 1.0 / DEFAULT_HZ);
  if (k->oaction != kid_unclimb) k->i = 14;
  if (k->oaction == kid_climb) k->i = 3;

  if (k->i == 0) {
    kid_hang (k);
    return false;
  }

  if (k->i == 14) {
    get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);
    struct rect r; actor_rect (&r, k, "KID", "CLIMB", "UNCLIMB");
    place_actor (k, &hanged_pos, r.c.x, r.c.y, "KID", "CLIMB", 13);
  }

  k->i--;

  k->fo.b = actor_bitmap (k, "KID", "CLIMB", k->i);
  k->fo.dx = -actor_dx (k, "KID", "CLIMB", k->i + 1);
  k->fo.dy = -actor_dy (k, "KID", "CLIMB", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* fall */
  if (is_falling (&k->f, _tf, +0, +0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* place on the ground */
  if (k->i >= 10) place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  keep_depressible_floor (k);
}
