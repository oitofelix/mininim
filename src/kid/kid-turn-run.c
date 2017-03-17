/*
  kid-turn-run.c -- kid turn run module;

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
kid_turn_run (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_turn_run;

  if (k->oaction != kid_turn_run)
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  k->f.flip = (k->f.dir == RIGHT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame_inv = true;
  next_frame (&k->f, &k->f, &k->fo);
  next_frame_inv = false;
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_turn_run) k->i = -1;

  if (k->i == 8) {
    kid_run (k);
    return false;
  }

  select_actor_frame (k, "KID", "TURN_RUN", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = k->cinertia = 0;

  /* collision */
  next_frame_inv = true;
  uncollide (&k->f, &k->fo, _bb, +0, -2, &k->fo, NULL);
  next_frame_inv = false;

  /* fall */
  if (! k->constrained_turn_run && is_falling (&k->f, _tb, -4, +0)) {
    if (k->i == 1) move_frame (&k->f, _tf, +0, +6, +6);
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* haptic */
  request_gamepad_rumble (0.5 - k->i / 8, 1.0 / DEFAULT_HZ);

  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -4, -27);
  else if (k->i == 1) update_depressible_floor (k, -9, -28);
  else if (k->i == 2) update_depressible_floor (k, -11, -29);
  else if (k->i == 3) update_depressible_floor (k, -6, -27);
  else if (k->i == 4) update_depressible_floor (k, -10, -31);
  else if (k->i == 5) update_depressible_floor (k, -17, -29);
  else keep_depressible_floor (k);
}
