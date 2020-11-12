/*
  kid-walk.c -- kid walk module;

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
  if (k->oaction != kid_walk) {
    k->i = -1;
    surveyo (_tb, +0, +0, pos, &k->f, NULL, &k->p, NULL);

    if (is_critical (&k->p))
      surveyo (_tf, +0, +0, pos, &k->f, NULL, &k->p, NULL);

    int dc = dist_collision (&k->f, _tf, +0, +0, NULL);

    if ((k->f.dir == LEFT && dc <= 4)
        || (k->f.dir == RIGHT && dc <= 10)) {
      kid_normal (k);
      return false;
    }
  }

  if (k->i == 11) {
    kid_normal (k);

    /* put kid nearest danger */
    while (k->ignore_danger == 1
           && ! is_in_danger (&k->p, &k->f, _tf, NULL))
      k->f.c.x += k->f.dir == LEFT ? -1 : +1;

    return false;
  }

  struct frame f = k->f;
  struct coord c0; _bb (&f, &c0);
  struct coord c;

  int back_threshold = 4;

  int i = k->i;

  do {
    k->f = f;

    while (k->ignore_danger < 2
           && is_in_danger (&k->p, &k->f, _tf, &k->misstep)) {
      if (k->ignore_danger == 0) k->ignore_danger = 1;
      if (i == -1) {
        if (k->misstep) kid_misstep (k);
        else kid_normal (k);
        return false;
      }
      k->f.c.x += k->f.dir == LEFT ? +1 : -1;
    }

    select_actor_frame (k, "KID", "WALK", k->i + 1);
    next_frame (&k->f, &f, &k->fo);

    _bb (&f, &c);
  } while (k->ignore_danger < 2
           && (is_in_danger (&k->p, &f, _tf, NULL)
            || (f.dir == LEFT ? c.x - c0.x > back_threshold
                : c.x - c0.x < -back_threshold))
           && k->i <= 10);

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* inertia */
  k->inertia = k->cinertia = 0;

  /* fall */
  if ((k->i < 6 && is_falling (&k->f, _bb, +0, +0)
       && is_falling (&k->f, _mbo, +0, +0))
      || (k->i >= 6 && is_falling (&k->f, _mbo, +0, +0)
          && is_falling (&k->f, _bf, +0, +0))) {
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
  int dx = 0;
  if (k->i == 6) update_depressible_floor (k, -3 + dx, -5);
  else if (k->i == 7) update_depressible_floor (k, 0 + dx, -6);
  else if (k->i == 10) update_depressible_floor (k, -4 + dx, -10);
  else keep_depressible_floor (k);
}
