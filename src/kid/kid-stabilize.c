/*
  kid-stabilize.c -- kid stabilize module;

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
kid_stabilize (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_stabilize;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! cutscene && ! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

void
kid_stabilize_collision (struct actor *k)
{
  play_audio (&hit_wall_audio, NULL, k->id);
  kid_haptic (k, KID_HAPTIC_COLLISION);

  k->action = kid_stabilize_collision;

  int dx = (k->f.dir == LEFT) ? PLACE_WIDTH + 18 : -PLACE_WIDTH + 24;
  int dy = +17;
  place_actor (k, &k->ci.kid_p, dx, dy, "KID", "STABILIZE", 0);

  kid_stabilize (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_stabilize) {
    k->i = -1, k->misstep = false;
    if (k->oaction == kid_stabilize_collision) {
      k->i = 0; k->collision = true;
    } else k->collision = false;
    if (k->oaction == kid_turn) k->collision = true;
  }

  if (! k->turn)
    k->turn = ((k->f.dir == RIGHT) && k->key.left)
      || ((k->f.dir == LEFT) && k->key.right);
  bool run = (((k->f.dir == RIGHT) && k->key.right)
              || ((k->f.dir == LEFT) && k->key.left))
    && ! k->key.shift;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);
  bool crouch = k->key.down;

  int dc = dist_collision (&k->f, _bf, +0, +0, &k->ci);
  int df = dist_fall (&k->f, false);

  if (k->i >= 0 && ! k->collision) {
    if (crouch) {
      kid_crouch (k);
      return false;
    } else if (jump) {
      kid_jump (k);
      return false;
    } else if (k->turn) {
      kid_turn (k);
      k->turn = false;
      return false;
    } else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH) {
      kid_start_run (k);
      return false;
    }
  }

  if (k->i == 3) {
    kid_normal (k);
    k->turn = false;
    return false;
  }

  select_actor_frame (k, "KID", "STABILIZE", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = k->cinertia = 0;

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)
      && is_falling (&k->f, _bb, +0, +0)) {
    kid_fall (k);
    return false;
  }

  /* collision */
  if (! k->collision)
    uncollide (&k->f, &k->fo, _bf, -12, -4, &k->fo, NULL);

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  if (k->collision && k->i == 1)
    update_depressible_floor (k, -13, -18);
  else keep_depressible_floor (k);
}
