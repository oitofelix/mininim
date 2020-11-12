/*
  guard-sword-hit.c -- guard sword hit module;

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
guard_sword_hit (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_sword_hit;
  g->f.flip = (g->f.dir == RIGHT) ?  ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  if (g->oaction != guard_sword_hit) {
    survey (_m, pos, &g->f, NULL, &g->p, NULL);
    g->i = -1;
    g->j = 0;
    g->refraction = g->skill.refraction;
  }

  if (g->i == 4) {
    int d = (g->f.dir == LEFT) ? +1 : -1;
    struct pos pb;
    prel (&g->p, &pb, 0, d);

    if (g->current_hp > 0) guard_sword_normal (g);
    else if (is_traversable (&pb)) {
        place_at_pos (&g->f, _m, &pb, &g->f.c);
        guard_fall (g);
    } else guard_die (g);

    return false;
  }

  select_actor_frame (g, NULL, "SWORD_HIT", g->i + 1);

  if (g->i == 0) g->j = 28;
  if (g->i == 1) g->j = 32;
  if (g->i == 2) g->j = 29;
  if (g->i == 3) g->j = 7;
  if (g->i == 4) g->j = 3;

  select_actor_xframe (g, NULL, "SWORD", g->j);

  /* if (g->i == 0) g->xf.dx = -12, g->xf.dy = +4; */
  /* if (g->i == 3) g->xf.dx = +0, g->xf.dy = +23; */

  /* if (g->type == SKELETON) g->xf.dy += -3; */
  /* if (g->type == SHADOW && g->i != 2) g->xf.dy += -2; */

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
  if (g->i == 3) update_depressible_floor (g, -3, -33);
  else if (g->i == 4) update_depressible_floor (g, -2, -24);
  else keep_depressible_floor (g);
}
