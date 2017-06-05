/*
  kid-stop-run.c -- kid stop run module;

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
kid_stop_run (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_stop_run;
  k->f.flip = (k->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_stop_run) {
    k->i = -1;
    k->constrained_turn_run = false;
  }

  bool turn_run = (k->f.dir == RIGHT) ? k->key.left : k->key.right;
  bool crouch = k->key.down;

  if (crouch) {
    kid_crouch (k);
    return false;
  }

  if (k->i == 3) {
    if (turn_run) kid_turn_run (k);
    else kid_stabilize (k);
    return false;
  }

  select_actor_frame (k, "KID", "STOP_RUN", k->i + 1);

  /* constrained turn run */
  if (k->oaction == kid_start_run && turn_run && k->i == 0) {
    struct pos ptf; surveyo (_tf, +0, +0, pos, &k->f, NULL, &ptf, NULL);
    if (is_constrained_pos (&ptf, &k->f)) {
      k->fo.dx += actor_dx (k, "KID", "STOP_RUN", "CONSTRAINED_TURN_RUN");
      k->constrained_turn_run = true;
    }
  }

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = 0;
  k->cinertia = 6;

  /* collision */
  if (k->f.dir == LEFT
      && uncollide (&k->f, &k->fo, _bf, +0, +0, NULL, &k->ci)
      && fg (&k->ci.tile_p) == MIRROR)
    uncollide (&k->f, &k->fo, _bf, +4, +0, &k->fo, &k->ci);
  else uncollide (&k->f, &k->fo, _bf,
                  COLLISION_FRONT_LEFT_NORMAL + k->i <= 2 ? -4 : 0,
                  COLLISION_FRONT_RIGHT_NORMAL,
                  &k->fo, NULL);

  /* fall */
  if (! k->constrained_turn_run && is_falling (&k->f, _tf, -4, -4)) {
    kid_fall (k);
    return false;
  }

  return true;
}

void
physics_out (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  if (k->i == 0) update_depressible_floor (k, -1, -5);
  else if (k->i == 2) update_depressible_floor (k, -2, -7);
  else keep_depressible_floor (k);

  /* sound and haptic */
  if (k->i == 1 || k->i == 3) {
    play_audio (&step_audio, NULL, k->id);
    kid_haptic (k, KID_HAPTIC_STEP);
  }
}
