/*
  kid-sword-walkf.c -- guard sword walk forward module;

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

static bool flow (struct actor *g);
static bool physics_in (struct actor *g);
static void physics_out (struct actor *g);

void
guard_sword_walkf (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_walkf;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_sword_walkf) g->i = -1;

  if (g->i == 2) {
    guard_sword_normal (g);
    return false;
  }

  select_actor_frame (g, NULL, "SWORD_WALKF", g->i + 1);

  if (g->i == 0) g->j = 8;
  if (g->i >= 1) g->j = 4;

  select_actor_xframe (g, NULL, "SWORD", g->j);

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_front_fight (g);

  /* fall */
  if (is_falling (&g->f, _mbo, +0, +0)) {
    g->xf.b = NULL;
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* place on the ground */
  place_on_the_ground (&g->f, &g->f.c);

  /* depressible floors */
  if (g->i == 2) update_depressible_floor (g, -3, -38);
  else keep_depressible_floor (g);
}
