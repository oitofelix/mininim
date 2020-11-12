/*
  kid-turn.c -- kid turn module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
kid_turn (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_turn;

  if (k->oaction != kid_turn)
    k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
  k->f.flip = (k->f.dir == LEFT) ? 0 : ALLEGRO_FLIP_HORIZONTAL;

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
  if (k->oaction != kid_turn)
    k->i = -1, k->hang = false;

  if (! k->turn)
    k->turn = ((k->f.dir == RIGHT) && k->key.left)
      || ((k->f.dir == LEFT) && k->key.right);
  bool run = ((k->f.dir == RIGHT) ? k->key.right : k->key.left)
    && ! k->key.shift;
  bool jump = ((k->f.dir == RIGHT) && k->key.right && k->key.up)
    || ((k->f.dir == LEFT) && k->key.left && k->key.up);
  bool crouch = k->key.down;

  if (k->i == 3) {
    int dc = dist_collision (&k->f, _bf, +0, +0, &k->ci);
    int df = dist_fall (&k->f, false);

    if (k->hang) kid_hang (k);
    else if (k->turn) {
      k->i = -1; k->turn = false;
      /* k->action = kid_normal; */
      k->f.dir = (k->f.dir == LEFT) ? RIGHT : LEFT;
      kid_turn (k);
    }
    else if (crouch) kid_crouch (k);
    else if (jump) kid_jump (k);
    else if (run && dc > PLACE_WIDTH && df > PLACE_WIDTH)
      kid_start_run (k);
    else kid_stabilize (k);

    return false;
  }

  if (k->oaction == kid_keep_sword) k->i = 2;

  /* hang */
  if (k->oaction == kid_fall
      || k->oaction == kid_jump
      || k->oaction == kid_run_jump
      || k->oaction == kid_hang_free
      || k->oaction == kid_hang_non_free) {
    k->i = 2, k->hang = true;
    int dx = (k->f.dir == LEFT) ? 7 : PLACE_WIDTH;
    int dy = +4;
    place_actor (k, &k->hang_pos, dx, dy, "KID", "TURN", 2);
  }

  select_actor_frame (k, "KID", "TURN", k->i + 1);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  bool turn = ((k->f.dir == RIGHT) && k->key.left)
    || ((k->f.dir == LEFT) && k->key.right);

  next_frame_inv = true;
  if (turn) uncollide_static
              (&k->f, &k->fo, _bf,
               COLLISION_FRONT_LEFT_NORMAL,
               COLLISION_FRONT_RIGHT_NORMAL,
               _bb,
               COLLISION_BACK_LEFT_NORMAL,
               COLLISION_BACK_RIGHT_NORMAL,
               &k->fo);
  next_frame_inv = false;

  /* fall */
  if (! k->hang
      && is_falling (&k->f, _bf, +0, +0)
      && is_falling (&k->f, _bb, +0, +0)) {
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* place on the ground */
  place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  keep_depressible_floor (k);
}
