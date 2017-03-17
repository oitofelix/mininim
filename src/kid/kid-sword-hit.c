/*
  kid-sword-hit.c -- kid sword walk forward module;

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
kid_sword_hit (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_sword_hit;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_sword_hit) {
    survey (_m, pos, &k->f, NULL, &k->p, NULL);
    k->i = -1;
    k->j = 0;
  }

  if (k->i == 4) {
    int d = (k->f.dir == LEFT) ? +1 : -1;
    struct pos pb;
    prel (&k->p, &pb, 0, d);

    if (k->current_lives > 0) kid_sword_normal (k);
    else if (is_traversable (&pb)) {
        place_at_pos (&k->f, _m, &pb, &k->f.c);
        kid_fall (k);
    } else kid_die (k);

    return false;
  }

  select_actor_frame (k, "KID", "SWORD_HIT", k->i + 1);

  if (k->i == 0) k->j = 28;
  if (k->i == 1) k->j = 32;
  if (k->i == 2) k->j = 29;
  if (k->i == 3) k->j = 7;
  if (k->i == 4) k->j = 17;

  select_actor_xframe (k, "KID", "SWORD", k->j);
  if (! k->has_sword) k->xf.b = NULL;

  if (k->i == 0) k->xf.dx = -12, k->xf.dy = +2;
  if (k->i == 1) k->xf.dy += -2;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide_back_fight (k);

  /* fall */
  if (is_falling (&k->f, _mbo, +0, +0)) {
    k->xf.b = NULL;
    kid_fall (k);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *k)
{
  /* depressible floors */
  if (k->i == 3) update_depressible_floor (k, -4, -33);
  else if (k->i == 4) update_depressible_floor (k, -1, -24);
  else keep_depressible_floor (k);
}
