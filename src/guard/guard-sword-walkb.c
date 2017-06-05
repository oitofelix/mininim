/*
  guard-sword-walkb.c -- guard sword walk backward module;

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

static bool flow (struct actor *g);
static bool physics_in (struct actor *g);
static void physics_out (struct actor *g);

void
guard_sword_walkb (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_walkb;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_sword_walkb) g->i = -1;

  if (g->i == 1) {
    guard_sword_normal (g);
    return false;
  }

  select_actor_frame (g, NULL, "SWORD_WALKB", g->i + 1);

  if (g->i == 0) g->j = 10;
  if (g->i == 1) g->j = 3;

  select_actor_xframe (g, NULL, "SWORD", g->j);

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_back_fight (g);

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
  if (g->i == 0) update_depressible_floor (g, 0, -34);
  else if (g->i == 1) update_depressible_floor (g, -1, -27);
  else keep_depressible_floor (g);
}
