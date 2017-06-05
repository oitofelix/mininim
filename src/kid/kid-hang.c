/*
  kid-hang.c -- kid hang module;

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
kid_hang (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_hang;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_hang)
    k->i = -1, k->hang_caller = k->oaction;

  struct rect r; actor_rect (&r, k, "KID", "HANG", "HANG");
  place_actor (k, &k->hang_pos, r.c.x, r.c.y, "KID", "HANG", 13);

  if (k->i == -1 && k->oaction != kid_unclimb) {
    k->fo.b = actor_bitmap (k, "KID", "HANG", 13);
    k->fo.dx = actor_dx (k, "KID", "HANG", 13);
    k->fo.dy = actor_dy (k, "KID", "HANG", 13);
  } else if (is_free (&k->hang_pos, k->f.dir)) {
    kid_hang_free (k);
    return false;
  } else {
    kid_hang_non_free (k);
    return false;
  }

  k->i++;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* intertia */
  k->inertia = k->cinertia = 0;

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  keep_depressible_floor (k);
}

bool
is_kid_hang (struct actor *k)
{
  return k->action == kid_hang || k->action == kid_hang_free
    || k->action == kid_hang_non_free;
}

bool
is_kid_hang_or_climb (struct actor *k)
{
  return is_kid_hang (k) || is_kid_climb (k);
}

bool
is_kid_hanging_at_pos (struct actor *k, struct pos *p)
{
  pos_f pf = (k->f.dir == LEFT) ? posf : pos;

  struct pos pbb, pr;
  survey (_bb, pf, &k->f, NULL, &pbb, NULL);

  int dir = (k->f.dir == LEFT) ? -1 : +1;
  return (is_kid_hang_or_climb (k)
          && peq (prel (&pbb, &pr, -1, dir), p));
}
