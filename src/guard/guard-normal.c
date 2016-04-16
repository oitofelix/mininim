/*
  guard-normal.c -- guard normal module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

static bool flow (struct anim *g);
static bool physics_in (struct anim *g);
static void physics_out (struct anim *g);

/* guard */
ALLEGRO_BITMAP *guard_normal_00;

/* fat guard */
ALLEGRO_BITMAP *fat_guard_normal_00;

/* vizier */
ALLEGRO_BITMAP *vizier_normal_00;

/* skeleton */
ALLEGRO_BITMAP *skeleton_normal_00;

/* shadow */
ALLEGRO_BITMAP *shadow_normal_00;

ALLEGRO_BITMAP *
get_guard_normal_bitmap (enum anim_type t)
{
  switch (t) {
  case GUARD: default: return guard_normal_00;
  case FAT_GUARD: return fat_guard_normal_00;
  case VIZIER: return vizier_normal_00;
  case SKELETON: return skeleton_normal_00;
  case SHADOW: return shadow_normal_00;
  }
}

void
load_guard_normal (void)
{
  /* guard */
  guard_normal_00 = load_bitmap (GUARD_NORMAL_00);

  /* fat guard */
  fat_guard_normal_00 = load_bitmap (FAT_GUARD_NORMAL_00);

  /* vizier */
  vizier_normal_00 = load_bitmap (VIZIER_NORMAL_00);

  /* skeleton */
  skeleton_normal_00 = load_bitmap (SKELETON_NORMAL_00);

  /* shadow */
  shadow_normal_00 = load_bitmap (SHADOW_NORMAL_00);
}

void
unload_guard_normal (void)
{
  /* guard */
  destroy_bitmap (guard_normal_00);

  /* fat guard */
  destroy_bitmap (fat_guard_normal_00);

  /* vizier */
  destroy_bitmap (vizier_normal_00);

  /* skeleton */
  destroy_bitmap (skeleton_normal_00);

  /* shadow */
  destroy_bitmap (shadow_normal_00);
}

void
guard_normal (struct anim *g)
{
  g->oaction = g->action;
  g->action = guard_normal;
  g->f.flip = (g->f.dir == RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0;

  if (! flow (g)) return;
  if (! physics_in (g)) return;
  next_frame (&g->f, &g->f, &g->fo);
  physics_out (g);
}

static bool
flow (struct anim *g)
{
  struct coord nc;
  struct pos np, pmt;

  bool vigilant = g->key.enter;

  if (g->oaction == guard_normal
      && g->current_lives <= 0) {
    survey (_mt, pos, &g->f, &nc, &pmt, &np);
    g->p = pmt;
    guard_die (g);
    return false;
  }

  if (g->oaction == guard_normal
      && vigilant) {
    guard_vigilant (g);
    return false;
  }

  g->fo.b = get_guard_normal_bitmap (g->type);
  g->fo.dx = g->fo.dy = +0;

  select_xframe (&g->xf, sword_frameset, 30);

  if (g->type == VIZIER) g->xf.dy += +4;
  if (g->type == SKELETON) g->xf.dx += -5, g->xf.dy += -6;

  return true;
}

static bool
physics_in (struct anim *g)
{
  struct coord nc; struct pos np, pmbo, pbb;

  /* inertia */
  g->inertia = g->cinertia = 0;

  /* fall */
  survey (_mbo, pos, &g->f, &nc, &pmbo, &np);
  survey (_bb, pos, &g->f, &nc, &pbb, &np);
  if (is_strictly_traversable (&pmbo)
      && is_strictly_traversable (&pbb)) {
    guard_fall (g);
    return false;
  }

  return true;
}

static void
physics_out (struct anim *g)
{
  /* depressible floors */
  update_depressible_floor (g, -7, -26);
}
