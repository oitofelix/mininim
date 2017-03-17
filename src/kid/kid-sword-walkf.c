/*
  kid-sword-walkf.c -- kid sword walk forward module;

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
kid_sword_walkf (struct actor *k)
{
  k->oaction = k->action;
  k->oi = k->i;
  k->action = kid_sword_walkf;
  k->f.flip = (k->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (k)) return;
  if (! physics_in (k)) return;
  next_frame (&k->f, &k->f, &k->fo);
  physics_out (k);
}

static bool
flow (struct actor *k)
{
  if (k->oaction != kid_sword_walkf) k->i = -1;

  if (k->i == 1) {
    kid_sword_normal (k);
    return false;
  }

  select_actor_frame (k, "KID", "SWORD_WALKF", k->i + 1);
  select_actor_xframe (k, "KID", "SWORD", 13);

  if (k->i == 0) k->xf.dx = -19;
  if (k->i == 1) k->xf.dx = -21;

  return true;
}

static bool
physics_in (struct actor *k)
{
  /* collision */
  uncollide_front_fight (k);

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
  if (k->i == 1) update_depressible_floor (k, -7, -35);
  else keep_depressible_floor (k);
}
