/*
  kid-climb.c -- kid climb module;

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
kid_climb (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_climb;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->i <= 4)
    request_gamepad_rumble (0.8, 1.0 / DEFAULT_HZ);

  if (k->oaction != kid_climb) {
    k->i = -1;
    k->wait = DOOR_WAIT_LOOK;
  }

  struct pos hanged_pos;
  get_hanged_pos (&k->hang_pos, k->f.dir, &hanged_pos);

  if (k->i == 14) {
    kid_crouch (k);
    return false;
  }

  if (k->i == -1) {
    struct rect r; actor_rect (&r, k, "KID", "CLIMB", "CLIMB");
    place_actor (k, &k->hang_pos, r.c.x, r.c.y, "KID", "CLIMB", 0);
  }

  if (k->wait == DOOR_WAIT_LOOK && k->i < 14) k->i++;

  select_actor_frame (k, "KID", "CLIMB", k->i);

  /* climbing a door when looking left should leave the kid farther
     from the edge to avoid collision with its grid */
  if (k->f.dir == LEFT
      && k->fo.dx < 0
      && fg (&hanged_pos) == DOOR)
    k->fo.dx += -1;

  if (k->i == 3 && k->wait < DOOR_WAIT_LOOK)
    k->fo.dx = 0, k->fo.dy = 0;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* fall */
  if (is_falling (&k->f, _tf, +0, +0)
      && is_falling (&k->f, _mt, +0, +0)) {
    kid_fall (k);
    return false;
  }

  /* door collision */
  struct pos ptf;
  enum tile_fg ctf;
  survey (_tf, pos, &k->f, NULL, &ptf, NULL);
  ctf = fg (&ptf);
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
physics_out (struct actor *k)
{
  /* place on the ground */
  if (k->i >= 10) place_on_the_ground (&k->f, &k->f.c);

  /* depressible floors */
  keep_depressible_floor (k);
}

bool
is_kid_climb (struct actor *k)
{
  return (k->action == kid_climb || k->action == kid_unclimb)
    /* && k->i <= 9 */;
}

bool
is_kid_successfully_climbing_at_pos (struct actor *k, struct pos *hang_pos,
                                     struct pos *p)
{
  struct pos np;
  int dir = (k->f.dir == LEFT) ? +1 : -1;
  return is_kid_successfully_climbing (k)
    && peq (prel (p, &np, +1, dir), hang_pos);
}

bool
is_kid_successfully_climbing (struct actor *k)
{
  return is_kid_climb (k) && k->i >= 4;
}
