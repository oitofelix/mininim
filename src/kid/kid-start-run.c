/*
  kid-start-run.c -- kid start run module;

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
kid_start_run (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_start_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_start_run) k->i = -1, k->misstep = false;

  bool run = (k->f.dir == RIGHT) ? k->key.right : k->key.left;
  bool turn_run = (k->f.dir == RIGHT) ? k->key.left : k->key.right;
  bool crouch = k->key.down;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);

  if (crouch) {
    kid_crouch (k);
    return false;
  }

  if (jump && k->i < 3) {
    kid_jump (k);
    return false;
  }

  if ((k->i == 3 || k->i == 4) && turn_run) {
    kid_stop_run (k);
    return false;
  }

  if (k->i == 5) {
    if (run) kid_run (k);
    else kid_stop_run (k);
    return false;
  }

  select_actor_frame (k, "KID", "START_RUN", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = 0;
  k->cinertia = 4;

  /* collision */
  if (uncollide (&k->f, &k->fo, _bf, -4, -4, NULL, &k->ci)) {
    kid_stabilize_collision (k);
    return false;
  }

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)
     || is_falling (&k->f, _tf, -4, -4)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  if (k->i == 5) update_depressible_floor (k, -16, -19);
  else keep_depressible_floor (k);
}
