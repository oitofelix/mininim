/*
  guard-normal.c -- guard normal module;

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
guard_normal (struct actor *g)
{
  g->oaction = g->action;
  g->oi = g->i;
  g->action = guard_normal;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct actor *g)
{
  struct pos pmt;

  bool vigilant = g->key.enter;

  if (g->oaction == guard_normal
      && g->current_lives <= 0
      && anim_cycle > 0) {
    survey (_mt, pos, &g->f, NULL, &pmt, NULL);
    g->p = pmt;
    guard_die (g);
    return false;
  }

  if (g->oaction == guard_normal
      && vigilant
      && anim_cycle > 0) {
    guard_sword_normal (g);
    return false;
  }

  g->fo.b = actor_bitmap (g, NULL, "NORMAL", 0);
  g->fo.dx = actor_bitmap_dx (g, NULL, "NORMAL", 0);
  g->fo.dy = actor_bitmap_dy (g, NULL, "NORMAL", 0);

  select_actor_xframe (g, NULL, "SWORD", 30);

  if (g->type == VIZIER) g->xf.dy += +4;
  if (g->type == SKELETON) g->xf.dx += -5, g->xf.dy += -6;

  return true;
}

static bool
physics_in (struct actor *g)
{
  /* collision */
  uncollide_static_neutral (g);

  /* inertia */
  g->inertia = g->cinertia = 0;

  /* fall */
  if (is_falling (&g->f, _mbo, +0, +0)) {
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct actor *g)
{
  /* depressible floors */
  update_depressible_floor (g, -7, -26);
}
